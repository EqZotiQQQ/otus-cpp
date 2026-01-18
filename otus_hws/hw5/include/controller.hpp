#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <expected>
#include <optional>
#include <string>

#include "editor.hpp"
#include "model.hpp"
#include "view.hpp"

struct CreateFileNameExpected {
    bool ok;
    std::optional<std::string> error_msg;
};

class Controller {
public:
    Controller(std::unique_ptr<Model>&& model, std::unique_ptr<View>&& view) : model_(std::move(model)), view_(std::move(view)) {
    }

    std::shared_ptr<Document> create_new_document(const std::string& doc_name) {
        return model_->create_new_document(doc_name);
    }

    void export_document_to_file(const std::string& doc_name, const std::filesystem::path& path) const {
        (void)doc_name;
        (void)path;
    }

    void add_rectangle_document(const std::string& doc_name, const Rectangle& rec) {
        model_->add_rectangle_document(doc_name, rec);
    }

    void add_line_document(const std::string& doc_name, const Line& line) {
        model_->add_line_document(doc_name, line);
    }

    void add_circle_document(const std::string& doc_name, const Circle& circle) {
        model_->add_circle_document(doc_name, circle);
    }

    std::string serialize(const std::string& doc_name) const {
        return  model_->serialize(doc_name);
    }

    std::string serialize(const std::string& doc_name, std::filesystem::path& path) const {
        std::string serialized = model_->serialize(doc_name);
        model_->create_new_document(path);
         
        return serialized;
    }


    

    // std::shared_ptr<Document> import_document_from_file(const std::filesystem::path& path) {

    // }

private:
    std::unique_ptr<Model> model_;
    std::unique_ptr<View> view_;
};
