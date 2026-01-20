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
#include <format>

#include <flatbuffers/flatbuffers.h>
#include "shapes_generated.h"

class Shape {
public:
    virtual ~Shape() {}
};

class Point {
public:
    explicit Point(int x, int y): x_(x), y_(y) {}

    explicit Point(const Shapes::Point& p): x_(p.x()), y_(p.y()) {}

    Shapes::Point to_flatbuffer_t() const {
        return Shapes::Point(x_, y_);
    }

    int x() const {
        return x_;
    }

    int y() const {
        return y_;
    }
private:
    int x_;
    int y_;
};

class Rectangle: public Shape {
public:
    explicit Rectangle(Point left_top, Point right_down): left_top_(left_top), right_down_(right_down) {}
    explicit Rectangle(int x1, int y1, int x2, int y2): Rectangle(Point{x1, y1}, Point{x2, y2}) {}
    explicit Rectangle(const Shapes::Rectangle& p): left_top_(p.left_up()), right_down_(p.right_down()) {}

    Shapes::Rectangle to_flatbuffer_t() const {
        return Shapes::Rectangle(left_top_.to_flatbuffer_t(), right_down_.to_flatbuffer_t());
    }

    Point left_top() const {
        return left_top_;
    }

    Point right_down() const {
        return right_down_;
    }
private:
    Point left_top_;
    Point right_down_;
};

class Line: public Shape {
public:
    explicit Line(Point start, Point end): start_(start), end_(end) {}
    explicit Line(int x1, int y1, int x2, int y2): Line(Point{x1, y1}, Point{x2, y2}) {}
    explicit Line(const Shapes::Line& p): start_(p.start()), end_(p.end()) {}

    Shapes::Line to_flatbuffer_t() const {
        return Shapes::Line(start_.to_flatbuffer_t(), end_.to_flatbuffer_t());
    }

    Point start() const {
        return start_;
    }

    Point end() const {
        return end_;
    }
private:
    Point start_;
    Point end_;
};

class Circle: public Shape {
public:
    explicit Circle(Point center, double radius): center_(center), radius_(radius) {}

    explicit Circle(const Shapes::Circle& p): center_(p.center()), radius_(p.radius()) {}

    Shapes::Circle to_flatbuffer_t() const {
        return Shapes::Circle(center_.to_flatbuffer_t(), radius_);
    }

    Point center() const {
        return center_;
    }

    double radius() const {
        return radius_;
    }
private:
    Point center_;
    double radius_;
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

    Document(const char* serialized_doc) {
        const Shapes::Document* document = Shapes::GetDocument(serialized_doc);
        fill_container<Circle>(*document->circles());
        fill_container<Line>(*document->lines());
        fill_container<Rectangle>(*document->rectangles());
    }

    template<typename V, typename U>
    void fill_container(const U& shapes_vec) {
        for (const auto& shape: shapes_vec) {
            add_shape(V(*shape));
        }
    }

    const char* serialize() const {
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
            } else {
                throw std::runtime_error("Bad serialization type");
            }
        }

        flatbuffers::FlatBufferBuilder builder;
        auto fb_circles = builder.CreateVectorOfStructs(circles);
        auto fb_lines = builder.CreateVectorOfStructs(lines);
        auto fb_rectangles = builder.CreateVectorOfStructs(rectangles);

        auto doc = Shapes::CreateDocument(builder, fb_circles, fb_lines, fb_rectangles);

        builder.Finish(doc);

        const char* serialized_doc = reinterpret_cast<const char*>(builder.GetBufferPointer());

        return serialized_doc;
    }

    template<typename ShapeName>
    void add_shape(const ShapeName& shape) {
        ObjectType key = type_to_enum<ShapeName>();
        document_objects_[key].push_back(std::make_shared<ShapeName>(shape));
    }

    std::string get_doc_name() const {
        return document_name_;
    }

    auto get_document_objects() const {
        return document_objects_;
    }

private:
    std::string document_name_{};
    std::unordered_map<ObjectType, std::vector<std::shared_ptr<Shape>>> document_objects_{}; // or use multiple vectors for each type of shape to remove vtable and shared_ptr
};

template <>
struct fmt::formatter<Point> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Point& p, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "Point(x={}, y={})", p.x(), p.y());
    }
};

template <>
struct fmt::formatter<Rectangle> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Rectangle& p, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "Rectangle(left_top={}, right_down={})", p.left_top(), p.right_down());
    }
};

template <>
struct fmt::formatter<Line> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Line& p, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "Line(start={}, end={})", p.start(), p.end());
    }
};

template <>
struct fmt::formatter<Circle> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Circle& p, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "Circle(center={}, radius={})", p.center(), p.radius());
    }
};


template <>
struct fmt::formatter<Document> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Document& d, FormatContext& ctx) const {
        const auto& map = d.get_document_objects();
        return fmt::format_to(
            ctx.out(), ""
          // "Document(\n  lines={},\n  rectangles={},\n  circles={}\n)",
          // map.at(ObjectType::LINE),
          // map.at(ObjectType::RECTANGLE),
          // map.at(ObjectType::CIRCLE)
        );
    }
};

template <typename T>
struct fmt::formatter<std::shared_ptr<T>> : fmt::formatter<T> {
    template <typename FormatContext>
    auto format(const std::shared_ptr<T>& p, FormatContext& ctx) const {
        if (!p) {
            return fmt::format_to(ctx.out(), "null");
        }
        return fmt::formatter<T>::format(*p, ctx);
    }
};

template <typename T>
struct fmt::formatter<std::vector<T>> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const std::vector<T>& vec, FormatContext& ctx) const {
        fmt::format_to(ctx.out(), "[");
        bool first = true;
        for (const auto& v : vec) {
            if (!first) fmt::format_to(ctx.out(), ", ");
            first = false;
            fmt::format_to(ctx.out(), "{}", v);
        }
        return fmt::format_to(ctx.out(), "]");
    }
};
