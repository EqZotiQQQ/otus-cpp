#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "editor.hpp"
#include "spdlog/spdlog.h"
#include "fmt/format.h"

class Model {
public:
    Model() {}

    auto create_new_document(const std::string& doc_name) {
        document_ = std::make_shared<Document>(doc_name);
        spdlog::info("Create new document {}", doc_name);
        notify_observer();
    }

    auto deserialize(const std::vector<uint8_t> serialized_doc) {
        document_ = std::make_shared<Document>(serialized_doc);
        spdlog::info("Deserialize doc");
        notify_observer();
    }

    int create_new_file_task(std::filesystem::path&& file_path) const {
        std::ofstream file(file_path, std::ios::binary);
        spdlog::info(fmt::format("File {} created", file_path.c_str()));
        return !file ? 1 : 0;
    }

    void add_rectangle_document(const Rectangle& rec1) {
        document_->add_shape(rec1);
        spdlog::info("Add rectangle");
        notify_observer();
    }

    void add_line_document(const Line& line) {
        document_->add_shape(line);
        spdlog::info("Add line");
        notify_observer();
    }

    void add_circle_document(const Circle& circle) {
        document_->add_shape(circle);
        spdlog::info("Add circle");
        notify_observer();
    }

    std::vector<uint8_t> serialize() const {
        return document_->serialize();
    }

    void set_observer_callback(std::function<void(const std::shared_ptr<Document>&)> cb) {
        cb_ = std::move(cb);
    }
private:
    void notify_observer() {
        if (cb_ && document_) {
            cb_(document_);
        }
    }
    
    std::function<void(const std::shared_ptr<Document>&)> cb_;
    std::shared_ptr<Document> document_;
};
