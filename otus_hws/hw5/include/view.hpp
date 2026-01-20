#pragma once

#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <expected>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <numbers>
#include <boost/type_index.hpp>
#include <ranges>
#include "editor.hpp"

class View {
public:
    template<typename Shape>
    void post_add_figure() {
        std::string type_name = boost::typeindex::type_id<Shape>().pretty_name();
        spdlog::info("Adding new shape: {}", type_name);
    }

    const char* print_serialized_data(const char* serialized_doc) const {
        spdlog::info("Serialized doc: {}", serialized_doc);
        return serialized_doc;
    }

    void print_new_doc_created(const std::string& doc_name) const {
        spdlog::info("Created doc: {}", doc_name);
    }

    const char* print_serialized_path_dst(const std::string& doc_name, std::filesystem::path& path, const char* serialized_from_model) const {
        spdlog::info("Doc {} saved to: {}", doc_name, path.root_path().c_str());
        return serialized_from_model;
    }

    std::shared_ptr<Document> convert_to_view_and_log(const std::shared_ptr<Document>& doc) const {
        spdlog::info("Deserialization done: {}", doc);
        return doc;
    }
};
