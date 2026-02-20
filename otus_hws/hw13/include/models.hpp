#pragma once

#include "model.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

#include <spdlog/spdlog.h>
#include <Eigen/Dense>

namespace fs = std::filesystem;

class LogRegModel : public Model {
private:
    Eigen::MatrixXd weights_;        // 10 x 785
    Eigen::VectorXd logits_;         // 10
    std::string model_name_ = "LogReg";

    static constexpr int kInputSize = 785;
    static constexpr int kNumClasses = 10;

public:
    explicit LogRegModel(const fs::path& path) {
        load_model(path);
        logits_.resize(kNumClasses);
    }

    int predict(const Eigen::VectorXd& features) override {
        logits_.noalias() = weights_ * features;

        Eigen::Index max_idx;
        logits_.maxCoeff(&max_idx);
        return static_cast<int>(max_idx);
    }

    double compute_accuracy(const fs::path& test_file) override {
        std::ifstream file(test_file);
        if (!file.is_open()) {
            spdlog::error("Cannot open test file: {}", test_file.string());
            return 0.0;
        }

        Eigen::VectorXd features(kInputSize);
        std::vector<double> buffer(kInputSize);

        int correct = 0;
        int total = 0;
        bool do_work = true;

        while (do_work) {
            for (int i = 0; i < kInputSize; ++i) {
                if (!(file >> buffer[i])) {
                    do_work = false;
                    break;
                }
            }

            int true_label = static_cast<int>(buffer[0]);

            features(0) = 1.0;
            Eigen::Map<Eigen::VectorXd>(features.data() + 1, kInputSize - 1) =
                Eigen::Map<Eigen::VectorXd>(&buffer[1], kInputSize - 1);

            int pred = predict(features);
            correct += (pred == true_label);
            ++total;
        }

        if (total == 0) return 0.0;

        double acc = static_cast<double>(correct) / total;
        spdlog::info("{} accuracy: {}/{} = {}", model_name_, correct, total, acc);
        return acc;
    }

    std::string name() const override {
        return model_name_;
    }

private:
    void load_model(const fs::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            spdlog::error("Cannot open LogReg model: {}", path.string());
            return;
        }

        weights_.resize(kNumClasses, kInputSize);

        for (int i = 0; i < kNumClasses; ++i) {
            for (int j = 0; j < kInputSize; ++j) {
                file >> weights_(i, j);
            }
        }

        spdlog::info("LogReg loaded: {}x{}", kNumClasses, kInputSize);
    }
};

class MLPModel : public Model {
private:
    Eigen::MatrixXd w1_;     // 128 x 784
    Eigen::MatrixXd w2_;     // 10 x 128

    Eigen::VectorXd tmp_;    // 784
    Eigen::VectorXd hidden_; // 128
    Eigen::VectorXd logits_; // 10

    std::string model_name_ = "MLPModel";

    static constexpr int kInputSize = 784;
    static constexpr int kHiddenSize = 128;
    static constexpr int kNumClasses = 10;

    const double inv_255_ = 1.0 / 255.0;

public:
    MLPModel(const fs::path& w1_path, const fs::path& w2_path) {
        load_model(w1_path, w2_path);

        tmp_.resize(kInputSize);
        hidden_.resize(kHiddenSize);
        logits_.resize(kNumClasses);
    }

    int predict(const Eigen::VectorXd& features) override {
        tmp_.noalias() = features;
        tmp_ *= inv_255_;

        hidden_.noalias() = w1_ * tmp_;

        hidden_ = 1.0 / (1.0 + (-hidden_.array()).exp());

        logits_.noalias() = w2_ * hidden_;

        Eigen::Index max_idx;
        logits_.maxCoeff(&max_idx);
        return static_cast<int>(max_idx);
    }

    double compute_accuracy(const fs::path& test_file) override {
        std::ifstream file(test_file);
        if (!file.is_open()) {
            spdlog::error("Cannot open test file: {}", test_file.string());
            return 0.0;
        }

        Eigen::VectorXd features(kInputSize);
        std::vector<double> buffer(kInputSize + 1);

        int correct = 0;
        int total = 0;
        bool do_work = true;

        while (do_work) {
            for (int i = 0; i < kInputSize + 1; ++i) {
                if (!(file >> buffer[i])) {
                    do_work = false;
                    break;
                }
            }

            int true_label = static_cast<int>(buffer[0]);

            Eigen::Map<Eigen::VectorXd>(features.data(), kInputSize) =
                Eigen::Map<Eigen::VectorXd>(&buffer[1], kInputSize);

            int pred = predict(features);
            correct += (pred == true_label);
            ++total;
        }


        if (total == 0) return 0.0;

        double acc = static_cast<double>(correct) / total;
        spdlog::info("{} accuracy: {}/{} = {}", model_name_, correct, total, acc);
        return acc;
    }

    std::string name() const override {
        return model_name_;
    }

private:
    void load_model(const fs::path& w1_path, const fs::path& w2_path) {
        std::ifstream f1(w1_path);
        std::ifstream f2(w2_path);

        if (!f1.is_open() || !f2.is_open()) {
            spdlog::error("Cannot load MLP weights: {}, {}",
                         w1_path.string(), w2_path.string());
            return;
        }

        Eigen::MatrixXd raw_w1(kInputSize, kHiddenSize);
        for (int i = 0; i < kInputSize; ++i)
            for (int j = 0; j < kHiddenSize; ++j)
                f1 >> raw_w1(i, j);

        Eigen::MatrixXd raw_w2(kHiddenSize, kNumClasses);
        for (int i = 0; i < kHiddenSize; ++i)
            for (int j = 0; j < kNumClasses; ++j)
                f2 >> raw_w2(i, j);

        w1_ = raw_w1.transpose();  // 128 x 784
        w2_ = raw_w2.transpose();  // 10 x 128

        spdlog::info("MLP mode: w1={}x{}, w2={}x{}",
                     kHiddenSize, kInputSize,
                     kNumClasses, kHiddenSize);
    }
};
