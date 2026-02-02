#pragma once


#include <vector>
#include <string>
#include <iostream>
#include <fstream>

// better apporach is to use compile-time template based approach and pass Args... args into concrete realization. But lets ommit it
struct IBulkSink {
    virtual ~IBulkSink() = default;
    virtual bool supports_log() const = 0;
    virtual bool supports_file() const = 0;
    virtual void flush(int64_t ts, const std::vector<std::string>&, size_t) = 0;
};

class FileBulkSink: public IBulkSink {
public:
    void flush(int64_t stamp, const std::vector<std::string>& commands, size_t suffix) override {
        std::ofstream file(std::to_string(stamp) + "_" + std::to_string(suffix) + ".txt");
        for (auto& c : commands) {
            file << c << "\n";
        }
    }
    bool supports_file() const override {return true;}
    bool supports_log() const override {return false;}

};

class ConsoleBulkSink: public IBulkSink {
public:
    bool supports_file() const override {return false;}
    bool supports_log() const override {return true;}

    void flush(int64_t, const std::vector<std::string>& commands, size_t) override {
        std::cout << "bulk: ";
        for (auto& c : commands) std::cout << c << " ";
        std::cout << "\n";
    }
};

class CaptureSink : public IBulkSink {
public:
    void flush(int64_t, const std::vector<std::string>& commands, size_t) override {
        std::vector<std::string> bulk;
        for (auto& c : commands) {
            bulk.push_back(c);
        }
        buffer_.push_back(std::move(bulk));
    }

    auto buffers() const {
        return buffer_;
    }
private:
    std::vector<std::vector<std::string>> buffer_;
};
