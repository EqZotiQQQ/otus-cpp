#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "view.hpp"
#include "model.hpp"
#include "controller.hpp"

int main() {
    std::unique_ptr<View> view{};
    std::unique_ptr<Model> model{};
    Controller controller{std::move(model), std::move(view)};

    // std::filesystem::path new_doc_path = std::filesystem::path("my_doc.txt");
    // controller.create_new_file_task(std::move(new_doc_path));

    controller.create_new_document("MyFirstDrawing");
    // controller.
    
    return 0;
}
