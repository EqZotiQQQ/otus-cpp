#pragma once

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#include <Eigen/Dense>

namespace fs = std::filesystem;

class Model {
public:
    virtual ~Model() = default;
    virtual int predict(const Eigen::VectorXd& features) = 0;
    virtual double compute_accuracy(const fs::path& test_file) = 0;
    virtual std::string name() const = 0;
};

std::unique_ptr<Model> create_logreg_model(const fs::path& path);
std::unique_ptr<Model> create_mlp_model(const fs::path& w1_path, const fs::path& w2_path);
