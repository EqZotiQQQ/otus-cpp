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
        const auto doc = model_->create_new_document(doc_name);
        view_->print_new_doc_created(doc->get_doc_name());
        return doc;
    }

    void export_document_to_file(const std::string& doc_name, const std::filesystem::path& path) const {
        (void)doc_name;
        (void)path;
    }

    void add_rectangle_document(const std::string& doc_name, const Rectangle& rec) {
        model_->add_rectangle_document(doc_name, rec);
        view_->post_add_figure<Rectangle>();
    }

    void add_line_document(const std::string& doc_name, const Line& line) {
        model_->add_line_document(doc_name, line);
        view_->post_add_figure<Line>();
    }

    void add_circle_document(const std::string& doc_name, const Circle& circle) {
        model_->add_circle_document(doc_name, circle);
        view_->post_add_figure<Circle>();
    }

    const char* serialize(const std::string& doc_name) const {
        const char* serialized_model = model_->serialize(doc_name);
        const char* serialized_view = view_->print_serialized_data(serialized_model);
        return serialized_view;
    }

    const char* serialize(const std::string& doc_name, std::filesystem::path& path) const {
        const char* serialized_model = model_->serialize(doc_name);
        const char* serialized_view = view_->print_serialized_path_dst(doc_name, path, serialized_model);
        return serialized_view;
    }

    std::shared_ptr<Document> deserialize(const std::string& doc_name, const char* serialized_doc) const {
        auto doc_model = model_->deserialize(doc_name, serialized_doc);
        auto doc_view = view_->convert_to_view_and_log(doc_model);
        return doc_view;
    }

    // std::shared_ptr<Document> import_document_from_file(const std::filesystem::path& path) {

    // }

private:
    std::unique_ptr<Model> model_;
    std::unique_ptr<View> view_;
};
