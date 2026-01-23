#include <cassert>
#include <memory>
#include <thread>

#include "editor.hpp"
#include "view.hpp"
#include "model.hpp"
#include "controller.hpp"

using namespace std::chrono_literals;

int main() {
    std::filesystem::path new_doc_path = std::filesystem::path("my_doc.txt");

    {
        std::shared_ptr<Model> model = std::make_shared<Model>();
        std::unique_ptr<View> view = std::make_unique<View>(model);
        view->start();

        Controller controller{std::move(model)};

        std::string doc_name = "MyFirstDrawing";
        controller.create_new_document(doc_name);

        std::this_thread::sleep_for(1s);
        Rectangle rec1{Point{1, 1}, Point{3,3}};
        controller.add_rectangle_document(rec1);

        std::this_thread::sleep_for(1s);
        Circle circle{Point{1, 1}, 42};
        controller.add_circle_document(circle);

        std::this_thread::sleep_for(1s);
        Line line{Point{1, 1}, Point{3,3}};
        controller.add_line_document(line);

        std::this_thread::sleep_for(1s);
        controller.export_document_to_file(new_doc_path);

        view->stop();
    }
    {
        std::shared_ptr<Model> model = std::make_shared<Model>();
        std::unique_ptr<View> view = std::make_unique<View>(model);
        view->start();
        Controller controller{std::move(model)};
        controller.import_document_from_file(new_doc_path);
        std::this_thread::sleep_for(1s);
        view->stop();
    }
    
    return 0;
}
