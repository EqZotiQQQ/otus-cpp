#pragma once

#include "model.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <spdlog/spdlog.h>
#include <Eigen/Dense>

namespace fs = std::filesystem;

constexpr int INPUT_SIZE = 784;
constexpr int BIAS_SIZE = 1;
constexpr int TOTAL_FEATURES = INPUT_SIZE + BIAS_SIZE;  // 785
constexpr int NUM_CLASSES = 10;
constexpr int MLP_HIDDEN_SIZE = 128;

class LogRegModel final : public Model {
private:
    Eigen::MatrixXd weights_;  // 10 x 785
    std::string name_ = "LogisticRegression";

public:
    explicit LogRegModel(const fs::path& path) {
        load_model(path);
    }

    int predict(const Eigen::VectorXd& features) noexcept override {
        Eigen::Index max_idx;
        (weights_ * features).maxCoeff(&max_idx);
        return static_cast<int>(max_idx);
    }

    double compute_accuracy(const fs::path& test_file) override {
        std::ifstream file(test_file);
        if (!file.is_open()) [[unlikely]] {
            spdlog::error("Cannot open test file: {}", test_file.string());
            return 0.0;
        }

        int correct = 0, total = 0;
        std::string line;
        line.reserve(8192);

        Eigen::VectorXd features(TOTAL_FEATURES);
        features(0) = 1.0;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            
            int true_label;
            iss >> true_label;
            
            for (int i = 0; i < INPUT_SIZE; ++i) {
                iss >> features(i + BIAS_SIZE);
            }

            int pred = predict(features);
            if (pred == true_label) [[likely]] {
                ++correct;
            }
            ++total;
        }

        double accuracy = static_cast<double>(correct) / total;
        spdlog::info("LogReg accuracy: {}/{} = {:.4f}", correct, total, accuracy);
        return accuracy;
    }

    std::string name() const override { return name_; }

private:
    void load_model(const fs::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) [[unlikely]] {
            spdlog::error("Cannot open LogReg model: {}", path.string());
            return;
        }

        Eigen::MatrixXd raw(INPUT_SIZE + BIAS_SIZE, NUM_CLASSES);
        
        for (int i = 0; i < TOTAL_FEATURES && file.good(); ++i) {
            for (int j = 0; j < NUM_CLASSES; ++j) {
                file >> raw(i, j);
            }
        }
        
        weights_ = raw.transpose();  // 10x785
        spdlog::info("LogReg loaded: {}x{} from {}", 
                    weights_.rows(), weights_.cols(), path.string());
    }
};

inline std::unique_ptr<Model> create_logreg_model(const fs::path& path) {
    return std::make_unique<LogRegModel>(path);
}

class MLPModel final : public Model {
private:
    Eigen::MatrixXd w1_;  // 784x128
    Eigen::MatrixXd w2_;  // 128x10
    std::string name_ = "MLP";
    
    static constexpr double NORMALIZATION_FACTOR = 1.0 / 255.0;

public:
    MLPModel(const fs::path& w1_path, const fs::path& w2_path) {
        load_model(w1_path, w2_path);
    }

    int predict(const Eigen::VectorXd& features) noexcept override {
        Eigen::VectorXd hidden = (w1_ * (features * NORMALIZATION_FACTOR))
            .unaryExpr([](double x) { return 1.0 / (1.0 + std::exp(-x)); });
        
        Eigen::VectorXd logits = w2_ * hidden;
        Eigen::Index max_idx;
        logits.maxCoeff(&max_idx);
        return static_cast<int>(max_idx);
    }
    

    double compute_accuracy(const fs::path& test_file) override {
        std::ifstream file(test_file);
        if (!file.is_open()) [[unlikely]] {
            spdlog::error("Cannot open test file: {}", test_file.string());
            return 0.0;
        }

        int correct = 0, total = 0;
        std::string line;
        line.reserve(8192);
        
        Eigen::VectorXd features(INPUT_SIZE);

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            
            int true_label;
            iss >> true_label;
            
            for (int i = 0; i < INPUT_SIZE; ++i) {
                iss >> features(i);
            }

            int pred = predict(features);
            if (pred == true_label) [[likely]] {
                ++correct;
            }
            ++total;
        }

        double accuracy = static_cast<double>(correct) / total;
        spdlog::info("MLP accuracy: {}/{} = {:.4f}", correct, total, accuracy);
        return accuracy;
    }

    std::string name() const override { return name_; }

private:
    static Eigen::VectorXd sigmoid(const Eigen::VectorXd& x) noexcept {
        return (1.0 / (1.0 + (-x.array()).exp())).matrix();
    }


    void load_model(const fs::path& w1_path, const fs::path& w2_path) {
        std::ifstream f1(w1_path), f2(w2_path);
        if (!f1.is_open() || !f2.is_open()) [[unlikely]] {
            spdlog::error("Cannot load MLP weights: {}, {}", 
                         w1_path.string(), w2_path.string());
            return;
        }

        w1_.resize(MLP_HIDDEN_SIZE, INPUT_SIZE);
        for (int i = 0; i < MLP_HIDDEN_SIZE && f1.good(); ++i) {
            for (int j = 0; j < INPUT_SIZE; ++j) {
                f1 >> w1_(i, j);
            }
        }
        
        w2_.resize(MLP_HIDDEN_SIZE, NUM_CLASSES);
        for (int i = 0; i < MLP_HIDDEN_SIZE && f2.good(); ++i) {
            for (int j = 0; j < NUM_CLASSES; ++j) {
                f2 >> w2_(i, j);
            }
        }

        spdlog::info("MLP loaded: w1={}x{}, w2={}x{}", 
                    w1_.rows(), w1_.cols(), w2_.rows(), w2_.cols());
    }
};

inline std::unique_ptr<Model> create_mlp_model(const fs::path& w1_path, const fs::path& w2_path) {
    return std::make_unique<MLPModel>(w1_path, w2_path);
}

std::unique_ptr<Model> create_model(const std::string& model_type,
                                   const fs::path& model_path);