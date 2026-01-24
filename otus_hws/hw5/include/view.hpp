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
#include "model.hpp"

class View {
public:
    View(std::shared_ptr<Model> model) {
        model->set_observer_callback([this](const std::shared_ptr<Document>& document){
            {
                std::lock_guard<std::mutex> lock(docMutex_);
                document_ = std::make_shared<Document>(*document);
            }
            cv_.notify_one();
        });
    }

    void start() {
        running_ = true;
        viewThread_ = std::thread([this]() { this->run(); });
    }

    void stop() {
        spdlog::info("Stopping drawing thread");
        {
            std::lock_guard<std::mutex> lock(docMutex_);
            running_ = false;
        }
        cv_.notify_one();
        if (viewThread_.joinable()) viewThread_.join();
    }

private:
    void run() {
        // sf::RenderWindow window(sf::VideoMode(800, 600), "MVC SFML View");
        std::unique_lock<std::mutex> lock(docMutex_);

        while (running_) {
            cv_.wait(lock, [this] { return !running_ || document_ != nullptr; });

            if (!running_) break;

            auto docCopy = document_;
            lock.unlock();

            // window.clear(sf::Color::White);

            if (docCopy) {
                auto shapes = docCopy->get_document_shapes();
                for (auto &s : shapes) {
                    std::visit([&](const auto& shapeData) {
                        using T = std::decay_t<decltype(shapeData)>;
                        if constexpr (std::is_same_v<T, Circle>) {
                            spdlog::info("Draw Circle: {}", s);
                            // sf::CircleShape shape(shapeData.radius());
                            // shape.setFillColor(sf::Color::Green);
                            // shape.setPosition(shapeData.center().x(), shapeData.center().y());
                            // window.draw(shape);
                        } else if constexpr (std::is_same_v<T, Line>) {
                            spdlog::info("Draw Line: {}", s);
                            // sf::Vertex line[] = {
                            //     sf::Vertex(sf::Vector2f(shapeData.start().x(), shapeData.start().y())),
                            //     sf::Vertex(sf::Vector2f(shapeData.end().x(), shapeData.end().y()))
                            // };
                            // window.draw(line, 2, sf::Lines);
                        } else if constexpr (std::is_same_v<T, Rectangle>) {
                            spdlog::info("Draw Rectangle: {}", s);
                            // sf::RectangleShape rect(sf::Vector2f(5, 5));
                            // rect.setFillColor(sf::Color::Blue);
                            // rect.setPosition(shapeData.left_top().x(), shapeData.left_top().y());
                            // window.draw(rect);
                        }
                    }, s);
                }
            }

            // window.display();
            lock.lock();
            document_.reset(); // this might cause errors but ok
        }
    }

    std::shared_ptr<Document> document_;
    std::mutex docMutex_;
    std::condition_variable cv_;
    std::thread viewThread_;
    bool running_;
};
