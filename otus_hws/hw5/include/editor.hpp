#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <expected>
#include <optional>
#include <string>
#include <unordered_map>

class Serializeable{
public:
    virtual void serialize() = 0;
    // virtual Serializeable* deserialize() = 0;

    virtual ~Serializeable() = default;
};

enum class ObjectType {
    CIRCLE = 0,
    LINE = 1
};

class Document: public Serializeable {
public:
    Document(const std::string& doc_name): document_name_(doc_name) {

    }

    void serialize() override {

    }

    ~Document() {};
    // Serializeable* deserialize() override {
        
    // }
private:
    std::string document_name_;
    std::unordered_map<ObjectType, std::vector<std::shared_ptr<Serializeable>>> document_objects_;
};
