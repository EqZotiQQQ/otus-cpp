#pragma once

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

class Serializeable{
public:
    virtual std::string serialize() const = 0;
    // virtual Serializeable* deserialize() = 0;

    virtual ~Serializeable() = default;
};

class Shape: public Serializeable {
    virtual int square() = 0;
};

class Point: public Serializeable {
public:
    explicit Point(int x, int y): x_(x), y_(y) {}

    std::string serialize() const override {
        return fmt::format("x={}:y={}", x_, y_);
    }

private:
    int x_;
    int y_;
};

class Rectangle: public Shape {
public:
    explicit Rectangle(Point left_top, Point right_down): left_top_(left_top), right_down_(right_down) {}
    explicit Rectangle(int x1, int y1, int x2, int y2): Rectangle(Point{x1, y1}, Point{x2, y2}) {}

    std::string serialize() const override {
        // todo flatbuffers or protobuf
        return fmt::format("left_top_={}:right_down_={}", left_top_.serialize(), right_down_.serialize());
    }

    int square() override {
        return 42;
    }

private:
    Point left_top_;
    Point right_down_;
};

class Line: public Shape {
public:
    explicit Line(Point start, Point end): start_(start), end_(end) {}
    explicit Line(int x1, int y1, int x2, int y2): Line(Point{x1, y1}, Point{x2, y2}) {}

    std::string serialize() const override {
        // todo flatbuffers or protobuf
        return fmt::format("start_={}:end_={}", start_.serialize(), end_.serialize());
    }

    int square() override {
        return 0;
    }

private:
    Point start_;
    Point end_;
};

class Circle: public Shape {
public:
    explicit Circle(Point center, int radius): center_(center), radius_(radius) {}

    std::string serialize() const override {
        // todo flatbuffers or protobuf
        return fmt::format("center_={}:radius_={}", center_.serialize(), radius_);
    }

    int square() override {
        if (square_.has_value()) {
            return *square_;
        } else {
            square_ = std::make_optional(std::numbers::pi * radius_ * radius_);
        }
        return *square_;
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

class Document: public Serializeable {
public:
    Document(const std::string& doc_name): document_name_(doc_name) {}

    std::string serialize() const override {
        std::string s = "";
        for (const auto& [k, v]: document_objects_) {
            if (v.empty()) {
                continue;
            }
            std::string type_name = boost::typeindex::type_id<decltype(*v.front())>().pretty_name();
            auto serialized_view = v | std::views::transform([](auto const& object) {
                    return object->serialize();
                });

            std::vector<std::string> serialized_group(
                serialized_view.begin(),
                serialized_view.end()
            );

            std::string out;
            if (!serialized_group.empty()) {
                out = serialized_group[0];
                for (size_t i = 1; i < serialized_group.size(); ++i) {
                    out += ", " + serialized_group[i];
                }
            }

            s += fmt::format(",object={}:data={}", type_name, out);
        }
        return s;
    }

    template<typename ShapeName>
    void add_shape(const ShapeName& shape) {
        std::string type_name = boost::typeindex::type_id<ShapeName>().pretty_name();
        spdlog::info("Adding new shape: {}", type_name);
        ObjectType key = type_to_enum<ShapeName>();
        document_objects_[key].push_back(std::make_shared<ShapeName>(shape));
    }

    ~Document() override {};
    // Serializeable* deserialize() override {
        
    // }
private:
    std::string document_name_{};
    std::unordered_map<ObjectType, std::vector<std::shared_ptr<Serializeable>>> document_objects_{};
};
