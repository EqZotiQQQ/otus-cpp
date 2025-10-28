#pragma once

#include <algorithm>
#include <memory>

#include "model.hpp"
#include "view.hpp"

class Controller {
public:
    Controller(std::unique_ptr<Model>&& model, std::unique_ptr<View>&& view) : model_(std::move(model)), view_(std::move(view)) {
    }

private:
    std::unique_ptr<Model> model_;
    std::unique_ptr<View> view_;
};
