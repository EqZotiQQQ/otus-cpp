#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>
#include <view.hpp>

#include "editor.hpp"
#include "spdlog/spdlog.h"
#include "fmt/format.h"

class Model {
public:
    Model() {}

    std::shared_ptr<Document> create_new_document(const std::string& doc_name) {
        auto doc = std::make_shared<Document>(doc_name);
        documents_.emplace(doc_name, doc);
        spdlog::info(fmt::format("Created document: {}", doc_name));
        return doc;
    }

    int create_new_file_task(std::filesystem::path&& file_path) const {
        std::ofstream file(file_path, std::ios::binary);
        spdlog::info(fmt::format("File {} created", file_path.c_str()));
        return !file ? 1 : 0;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Document>> documents_;
    std::vector<std::shared_ptr<View>> views_;
};
