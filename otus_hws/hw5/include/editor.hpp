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

#include <flatbuffers/flatbuffers.h>
#include "shapes_generated.h"

class Shape {
public:
    virtual ~Shape() {}
    virtual int square() = 0;
};

class Point {
public:
    explicit Point(int x, int y): x_(x), y_(y) {}

    Shapes::Point to_flatbuffer_t() const {
        return Shapes::Point(x_, y_);
    }

private:
    int x_;
    int y_;
};

class Rectangle: public Shape {
public:
    explicit Rectangle(Point left_top, Point right_down): left_top_(left_top), right_down_(right_down) {}
    explicit Rectangle(int x1, int y1, int x2, int y2): Rectangle(Point{x1, y1}, Point{x2, y2}) {}

    int square() {
        return 42;
    }

    Shapes::Rectangle to_flatbuffer_t() const {
        return Shapes::Rectangle(left_top_.to_flatbuffer_t(), right_down_.to_flatbuffer_t());
    }
private:
    Point left_top_;
    Point right_down_;
};

class Line: public Shape {
public:
    explicit Line(Point start, Point end): start_(start), end_(end) {}
    explicit Line(int x1, int y1, int x2, int y2): Line(Point{x1, y1}, Point{x2, y2}) {}

    int square() {
        return 0;
    }

    Shapes::Line to_flatbuffer_t() const {
        return Shapes::Line(start_.to_flatbuffer_t(), end_.to_flatbuffer_t());
    }
private:
    Point start_;
    Point end_;
};

class Circle: public Shape {
public:
    explicit Circle(Point center, int radius): center_(center), radius_(radius) {}

    int square() override {
        if (square_.has_value()) {
            return *square_;
        } else {
            square_ = std::make_optional(std::numbers::pi * radius_ * radius_);
        }
        return *square_;
    }

    Shapes::Circle to_flatbuffer_t() const {
        return Shapes::Circle(center_.to_flatbuffer_t(), radius_);
    }
private:
    Point center_;
    int radius_;
    std::optional<double> square_;
};


enum class ObjectType {
    CIRCLE = 0,
    LINE = 1,
    RECTANGLE = 2
};

template<typename T>
static inline ObjectType type_to_enum() {
    if constexpr (std::is_same_v<T, Circle>) {
        return ObjectType::CIRCLE;
    } else if constexpr (std::is_same_v<T, Line>) {
        return ObjectType::LINE;
    } else if constexpr (std::is_same_v<T, Rectangle>) {
        return ObjectType::RECTANGLE;
    } else {
        std::string type_name = boost::typeindex::type_id<T>().pretty_name();
        throw std::runtime_error(fmt::format("Bad type: {}", type_name));
    }
}

class Document {
public:
    Document(const std::string& doc_name): document_name_(doc_name) {}

    std::string serialize() const {
        std::vector<Shapes::Circle> circles;
        circles.reserve(document_objects_.contains(ObjectType::CIRCLE) ? document_objects_.at(ObjectType::CIRCLE).size() : 0);
        std::vector<Shapes::Line> lines;
        lines.reserve(document_objects_.contains(ObjectType::LINE) ? document_objects_.at(ObjectType::LINE).size() : 0);
        std::vector<Shapes::Rectangle> rectangles;
        rectangles.reserve(document_objects_.contains(ObjectType::RECTANGLE) ? document_objects_.at(ObjectType::RECTANGLE).size() : 0);

        for (const auto& [obj_type, objects]: document_objects_) {
            if constexpr (std::is_same_v<decltype(obj_type), Circle>) {
                for (const auto& circle: objects) {
                    const auto ptr = std::dynamic_pointer_cast<Circle>(circle);
                    if (ptr) {
                        circles.push_back(ptr->to_flatbuffer_t()); // error due to base class doesn't have this method but i dont know how to declare it correct
                    }
                }
            } else if constexpr (std::is_same_v<decltype(obj_type), Line>) {
                for (const auto& line: objects) {
                    const auto ptr = std::dynamic_pointer_cast<Line>(line);
                    if (ptr) {
                        lines.push_back(ptr->to_flatbuffer_t()); // error due to base class doesn't have this method but i dont know how to declare it correct
                    }
                }
            } else if constexpr (std::is_same_v<decltype(obj_type), Rectangle>) {
                for (const auto& circle: objects) {
                    const auto ptr = std::dynamic_pointer_cast<Rectangle>(circle);
                    if (ptr) {
                        rectangles.push_back(ptr->to_flatbuffer_t()); // error due to base class doesn't have this method but i dont know how to declare it correct
                    }
                }
            }
        }

        flatbuffers::FlatBufferBuilder builder;
        auto fb_circles = builder.CreateVectorOfStructs(circles);
        auto fb_lines = builder.CreateVectorOfStructs(lines);
        auto fb_rectangles = builder.CreateVectorOfStructs(rectangles);

        auto doc = Shapes::CreateDocument(builder, fb_circles, fb_lines, fb_rectangles);

        builder.Finish(doc);

        std::string serialized_doc = reinterpret_cast<const char*>(builder.GetBufferPointer());

        return serialized_doc;
    }

    template<typename ShapeName>
    void add_shape(const ShapeName& shape) {
        std::string type_name = boost::typeindex::type_id<ShapeName>().pretty_name();
        spdlog::info("Adding new shape: {}", type_name);
        ObjectType key = type_to_enum<ShapeName>();
        document_objects_[key].push_back(std::make_shared<ShapeName>(shape));
    }

private:
    std::string document_name_{};
    std::unordered_map<ObjectType, std::vector<std::shared_ptr<Shape>>> document_objects_{}; // or use multiple vectors for each type of shape to remove vtable and shared_ptr
};
