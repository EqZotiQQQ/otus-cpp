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
    Controller(const std::shared_ptr<Model>& model) : model_(model) {
    }

    void create_new_document(const std::string& doc_name) {
        model_->create_new_document(doc_name);
    }

    void add_rectangle_document(const Rectangle& rec) {
        model_->add_rectangle_document(rec);
    }

    void add_line_document(const Line& line) {
        model_->add_line_document(line);
    }

    void add_circle_document(const Circle& circle) {
        model_->add_circle_document(circle);
    }

    void serialize() const {
        std::vector<uint8_t> serialized = model_->serialize();
    }

    void serialize(std::filesystem::path& path) const {
        std::vector<uint8_t> serialized_model = model_->serialize();
        dump_to_file(path, serialized_model);
    }

    void deserialize(std::vector<uint8_t> serialized_doc) const {
        model_->deserialize(serialized_doc);
    }

    void serialize_and_deserialize() {
        std::vector<uint8_t> serialized_doc = model_->serialize();
        model_->deserialize(serialized_doc);
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

    void export_document_to_file(const std::filesystem::path& path) const {
        const std::vector<uint8_t> serialized = model_->serialize();
        dump_to_file(path, serialized);
    }

    void import_document_from_file(const std::filesystem::path& path) {
        std::vector<uint8_t> buffer = Controller::load_from_file(path);
        model_->deserialize(buffer);
    }

private:
    std::shared_ptr<Model> model_;
};
