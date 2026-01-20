#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "editor.hpp"
#include "view.hpp"
#include "model.hpp"
#include "controller.hpp"

int main() {
    std::unique_ptr<View> view = std::make_unique<View>(View{});
    std::unique_ptr<Model> model = std::make_unique<Model>(Model{});
    Controller controller{std::move(model), std::move(view)};

    std::filesystem::path new_doc_path = std::filesystem::path("my_doc.txt");

    std::string doc_name = "MyFirstDrawing";
    controller.create_new_document(doc_name);

    
    Rectangle rec1{Point{1, 1}, Point{3,3}};
    controller.add_rectangle_document(doc_name, rec1);

    Circle circle{Point{1, 1}, 42};
    controller.add_circle_document(doc_name, circle);

    Line line{Point{1, 1}, Point{3,3}};
    controller.add_line_document(doc_name, line);

    controller.export_document_to_file(doc_name, new_doc_path);

    const char* serialized_doc = controller.serialize(doc_name);
    spdlog::info("Serialized doc: {}", serialized_doc);

    controller.deserialize("MyFirstDrawing", serialized_doc);

    // controller.import_document_from_file(new_doc_path);
    // controller
    
    return 0;
}
