#pragma once

#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <boost/type_index.hpp>
#include "editor.hpp"

class View {
public:
    template<typename Shape>
    void post_add_figure() {
        std::string type_name = boost::typeindex::type_id<Shape>().pretty_name();
        spdlog::info("Adding new shape: {}", type_name);
    }

    void print_serialized_data(const std::vector<uint8_t>& serialized_doc) const {
        spdlog::info("Serialized doc: {}", *serialized_doc.data());
    }

    void show_doc(const std::shared_ptr<Document>& doc) const {
        spdlog::info("Doc to be shown: {}", *doc);
    }

    void print_serialized_path_dst(const std::string& doc_name, std::filesystem::path& path, const std::vector<uint8_t> serialized_from_model) const {
        (void)serialized_from_model;
        spdlog::info("Doc {} saved to: {}", doc_name, path.root_path().c_str());
    }

    void draw_doc(const std::shared_ptr<Document>& doc) const {
        spdlog::info("Deserialization done: {}", *doc);
    }

    void notify_user_about_success_dump(const std::string& doc_name, const std::filesystem::path& path) const {
        spdlog::info("Document {} successfully dumped to {}", doc_name, path.c_str());
    }
};
