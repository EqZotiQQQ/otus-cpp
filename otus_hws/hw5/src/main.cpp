#include <cassert>
#include <memory>

#include "editor.hpp"
#include "view.hpp"
#include "model.hpp"
#include "controller.hpp"

int main() {
    std::filesystem::path new_doc_path = std::filesystem::path("my_doc.txt");

    {
        std::unique_ptr<View> view = std::make_unique<View>(View{});
        std::unique_ptr<Model> model = std::make_unique<Model>(Model{});
        Controller controller{std::move(model), std::move(view)};

        std::string doc_name = "MyFirstDrawing";
        controller.create_new_document(doc_name);

        Rectangle rec1{Point{1, 1}, Point{3,3}};
        controller.add_rectangle_document(doc_name, rec1);

        Circle circle{Point{1, 1}, 42};
        controller.add_circle_document(doc_name, circle);

        Line line{Point{1, 1}, Point{3,3}};
        controller.add_line_document(doc_name, line);

        controller.export_document_to_file(doc_name, new_doc_path);
    }
    {
        std::unique_ptr<View> view = std::make_unique<View>(View{});
        std::unique_ptr<Model> model = std::make_unique<Model>(Model{});
        Controller controller{std::move(model), std::move(view)};
        controller.import_document_from_file(new_doc_path);
    }
    
    return 0;
}
