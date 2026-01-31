#include <filesystem>
#include <format>
#include <unordered_map>
#include <fstream>

namespace fs = std::filesystem;


struct FileHandle {
    fs::path path;
    std::ifstream stream;

    explicit FileHandle(const fs::path& p)
        : path(p), stream(p, std::ios::binary) {
        if (!stream) {
            throw std::runtime_error(std::format("Failed to open file {}", p.string()));
        }
    }

    void seek_block(size_t block_index, size_t block_size) {
        std::uintmax_t offset = block_index * block_size;
        stream.clear();
        stream.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
    }

    std::streamsize read_block(char* buf, size_t buf_size) {
        stream.read(buf, static_cast<std::streamsize>(buf_size));
        return stream.gcount();
    }
};


class FilePool {
    // keep sterams
    // we do not want to open files each cycle, we want reduce syscalls count
public:
    FileHandle& get(const fs::path& path) {
        auto it = pool.find(path);
        if (it == pool.end()) {
            auto [new_it, _] = pool.emplace(path, FileHandle{path});
            return new_it->second;
        }
        return it->second;
    }

private:
    std::unordered_map<fs::path, FileHandle> pool;
};

