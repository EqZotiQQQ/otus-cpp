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

private:
    std::unique_ptr<Model> model_;
    std::unique_ptr<View> view_;
};
