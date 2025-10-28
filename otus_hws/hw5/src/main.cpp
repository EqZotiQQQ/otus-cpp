#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "controller.hpp"

int main() {
    std::unique_ptr<View> view{};
    std::unique_ptr<Model> model{};
    Controller controller{std::move(model), std::move(view)};
    return 0;
}
