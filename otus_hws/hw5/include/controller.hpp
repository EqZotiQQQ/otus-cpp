#pragma once

#include <spdlog/common.h>
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



class Controller {
public:
    Controller(std::unique_ptr<Model>&& model, std::unique_ptr<View>&& view) : model_(std::move(model)), view_(std::move(view)) {
    }

    void create_new_document(const std::string& doc_name) {
        const std::shared_ptr<Document> doc = model_->create_new_document(doc_name);
        view_->show_doc(doc);
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

    void serialize(const std::string& doc_name) const {
        std::vector<uint8_t> serialized = model_->serialize(doc_name);
        view_->print_serialized_data(serialized);
    }

    void serialize(const std::string& doc_name, std::filesystem::path& path) const {
        std::vector<uint8_t> serialized_model = model_->serialize(doc_name);
        view_->print_serialized_path_dst(doc_name, path, serialized_model);
    }

    void deserialize(std::vector<uint8_t> serialized_doc) const {
        std::shared_ptr<Document> doc = model_->deserialize(serialized_doc);
        view_->draw_doc(doc);
    }

    void serialize_and_deserialize(const std::string& doc_name) {
        std::vector<uint8_t> serialized_doc = model_->serialize(doc_name);
        std::shared_ptr<Document> doc = model_->deserialize(serialized_doc);
    }

    static void dump_to_file(const std::filesystem::path& path, const std::vector<uint8_t>& serialized) {
        std::ofstream out(path, std::ios::binary);
        if (!out)
            throw std::runtime_error("Failed to open file");

        out.write(reinterpret_cast<const char*>(serialized.data()), static_cast<std::streamsize>(serialized.size()));
    }

    static std::vector<uint8_t> load_from_file(const std::filesystem::path& path) {
        std::ifstream infile(path, std::ios::binary);
        
        if (!infile) {
            throw fmt::format("Failed to open doc stored at file {} :(", path.c_str());
        }
        infile.seekg(0, std::ios::end);
        const std::streamsize size = infile.tellg();
        infile.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(size);

        if (!infile.read(reinterpret_cast<char*>(buffer.data()), size)) {
            throw fmt::format("Failed to read doc stored at file {} :(", path.c_str());
        }

        return buffer;
    }

    void export_document_to_file(const std::string& doc_name, const std::filesystem::path& path) const {
        const std::vector<uint8_t> serialized = model_->serialize(doc_name);
        dump_to_file(path, serialized);
        view_->notify_user_about_success_dump(doc_name, path);
    }

    void import_document_from_file(const std::filesystem::path& path) {
        std::vector<uint8_t> buffer = Controller::load_from_file(path);
        std::shared_ptr<Document> deserialized = model_->deserialize(buffer);
        view_->show_doc(deserialized);
    }

private:
    std::unique_ptr<Model> model_;
    std::unique_ptr<View> view_;
};
