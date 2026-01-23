#pragma once

#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <spdlog/spdlog.h>
#include <optional>
#include <string>
#include <vector>
#include <boost/type_index.hpp>
#include <fmt/format.h>
#include <flatbuffers/flatbuffers.h>
#include "shapes_generated.h"


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

class Rectangle {
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

class Line {
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

class Circle {
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

using ShapeVariant = std::variant<Circle, Line, Rectangle>;

class Document {
public:
    Document(const std::string& doc_name): document_name_(doc_name) {}


    Document(const std::vector<uint8_t>& serialized_doc) {
        const Shapes::Document* document = Shapes::GetDocument(serialized_doc.data());

        for (const auto* c : *document->circles())
            shapes_.emplace_back(Circle{*c});

        for (const auto* l : *document->lines())
            shapes_.emplace_back(Line{*l});

        for (const auto* r : *document->rectangles())
            shapes_.emplace_back(Rectangle{*r});

        document_name_ = document->doc_name()->str();
    }

    std::vector<uint8_t> serialize() const {
        std::vector<Shapes::Circle> circles;
        std::vector<Shapes::Line> lines;
        std::vector<Shapes::Rectangle> rectangles;

        for (const auto& shape : shapes_) {
            std::visit([&](const auto& s) {
                using T = std::decay_t<decltype(s)>;
                if constexpr (std::is_same_v<T, Circle>)
                    circles.push_back(s.to_flatbuffer_t());
                else if constexpr (std::is_same_v<T, Line>)
                    lines.push_back(s.to_flatbuffer_t());
                else if constexpr (std::is_same_v<T, Rectangle>)
                    rectangles.push_back(s.to_flatbuffer_t());
            }, shape);
        }


        flatbuffers::FlatBufferBuilder builder;

        auto fb_circles    = builder.CreateVectorOfStructs(circles);
        auto fb_lines      = builder.CreateVectorOfStructs(lines);
        auto fb_rectangles = builder.CreateVectorOfStructs(rectangles);
        auto fb_doc_name = builder.CreateString(document_name_);

        auto doc = Shapes::CreateDocument(builder, fb_doc_name, fb_circles, fb_lines, fb_rectangles);
        builder.Finish(doc);

        const uint8_t* data = builder.GetBufferPointer();
        size_t size = builder.GetSize();

        return std::vector<uint8_t>(data, data + size);
    }

    template<typename ShapeT>
    void add_shape(ShapeT&& shape) {
        shapes_.emplace_back(std::forward<ShapeT>(shape));
    }

    std::string get_doc_name() const {
        return document_name_;
    }

    auto get_document_shapes() const {
        return shapes_;
    }

private:
    std::string document_name_{};
    std::vector<ShapeVariant> shapes_;
};

template <>
struct fmt::formatter<Point> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Point& p, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "Point(x={}, y={})", p.x(), p.y());
    }
};

template <>
struct fmt::formatter<Rectangle> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Rectangle& r, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(),
            "Rectangle(left_top={}, right_down={})",
            r.left_top(), r.right_down());
    }
};

template <>
struct fmt::formatter<Line> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Line& l, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(),
            "Line(start={}, end={})",
            l.start(), l.end());
    }
};

template <>
struct fmt::formatter<Circle> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Circle& c, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(),
            "Circle(center={}, radius={})",
            c.center(), c.radius());
    }
};

template <>
struct fmt::formatter<ShapeVariant> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const ShapeVariant& v, FormatContext& ctx) const {
        std::visit([&](const auto& s) {
            fmt::format_to(ctx.out(), "{}", s);
        }, v);
        return ctx.out();
    }
};

template <>
struct fmt::formatter<Document> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Document& d, FormatContext& ctx) const {
        fmt::format_to(ctx.out(), "Document(\"{}\") [\n", d.get_doc_name());
        for (const auto& s : d.get_document_shapes()) {
            fmt::format_to(ctx.out(), "  {},\n", s);
        }
        fmt::format_to(ctx.out(), "]");
        return ctx.out();
    }
};
