#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "view.hpp"
#include "editor.hpp"
#include "spdlog/spdlog.h"
#include "fmt/format.h"

class Model {
public:
    Model() {}

    auto create_new_document(const std::string& doc_name) {
        auto doc = std::make_shared<Document>(doc_name);
        documents_.emplace(std::make_pair(doc_name, doc));
        return doc;
    }

    auto deserialize(const std::string& doc_name, const char* serialized_doc) {
        std::shared_ptr<Document> deserialized_doc = std::make_shared<Document>(Document(serialized_doc));
        documents_[doc_name] = deserialized_doc;
        return deserialized_doc;
    }

    int create_new_file_task(std::filesystem::path&& file_path) const {
        std::ofstream file(file_path, std::ios::binary);
        spdlog::info(fmt::format("File {} created", file_path.c_str()));
        return !file ? 1 : 0;
    }

    void add_rectangle_document(const std::string& doc_name, const Rectangle& rec1) {
        if (documents_.contains(doc_name)) {
            documents_[doc_name]->add_shape(rec1);
        } else {
            std::string err_s = fmt::format("Failed to add {}. Document {} not found!", "Rectangle", doc_name);
            spdlog::error(err_s);
            throw std::runtime_error(err_s);
        }
    }

    void add_line_document(const std::string& doc_name, const Line& line) {
        if (documents_.contains(doc_name)) {
            documents_[doc_name]->add_shape(line);
        } else {
            std::string err_s = fmt::format("Failed to add {}. Document {} not found!", "Line", doc_name);
            spdlog::error(err_s);
            throw std::runtime_error(err_s);
        }
    }

    void add_circle_document(const std::string& doc_name, const Circle& circle) {
        if (documents_.contains(doc_name)) {
            documents_[doc_name]->add_shape(circle);
        } else {
            std::string err_s = fmt::format("Failed to add {}. Document {} not found!", "Circle", doc_name);
            spdlog::error(err_s);
            throw std::runtime_error(err_s);
        }
    }

    const char* serialize(const std::string& doc_name) const {
        if (documents_.contains(doc_name)) {
            return documents_.at(doc_name)->serialize();
        } else {
            std::string err_s = fmt::format("Failed to serialize {} document due to its not found!", doc_name);
            spdlog::error(err_s);
            throw std::runtime_error(err_s);
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Document>> documents_{};
    std::vector<std::shared_ptr<View>> views_{};
};
