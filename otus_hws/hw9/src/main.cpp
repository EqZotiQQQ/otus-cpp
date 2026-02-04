#include "async.hpp"

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>
#include <random>


std::vector<std::string> make_big_input(char prefix, int commands, int chunk_size) {
    std::vector<std::string> chunks;
    chunks.reserve(commands / chunk_size + 1);

    std::string current;
    current.reserve(chunk_size * 16);

    for (int i = 0; i < commands; ++i) {
        current += prefix;
        current += std::to_string(i);
        current += "\n";

        if ((i + 1) % chunk_size == 0) {
            chunks.push_back(current);
            current.clear();
        }
    }

    if (!current.empty())
        chunks.push_back(current);

    return chunks;
}

void producer(int id, std::size_t bulk_size, const std::vector<std::string>& chunks) {
    void* ctx = connect(bulk_size);

    std::mt19937 rng(id * 1337);
    std::uniform_int_distribution<int> sleep_ms(0, 5);

    for (const auto& chunk : chunks) {
        receive(ctx, chunk.c_str(), chunk.size());

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms(rng)));
    }

    disconnect(ctx);

    std::cout << "Producer " << id << " finished (" << chunks.size() << " chunks)\n";
}

int main() {
    std::cout << "First block" << std::endl;
    {
        void* ctx = connect(3);

        const char* data1 = "cmd1\ncmd2\n";
        receive(ctx, data1, std::strlen(data1));

        const char* data2 = "cmd3\ncmd4\ncmd5\n";
        receive(ctx, data2, std::strlen(data2));

        disconnect(ctx);

    }

    std::cout << "Second block" << std::endl;
    {
        void* ctx = connect(3);

        const char* data1 = "cmd1\ncmd2\n{\ncmd3\ncmd4\n}\n{\ncmd5\ncmd6\n{\n";
        receive(ctx, data1, std::strlen(data1));

        const char* data2 = "cmd7\ncmd8\n}\ncmd9\n}\n{\ncmd10\ncmd11\n";
        receive(ctx, data2, std::strlen(data2));

        disconnect(ctx);
    }

    std::cout << "Third block" << std::endl;
    {
        auto chunks1 = make_big_input('A', 10000, 50);
        auto chunks2 = make_big_input('B', 12000, 40);
        auto chunks3 = make_big_input('C', 8000,  25);
        auto chunks4 = make_big_input('D', 15000, 60);

        std::cout << "Chunks generated:" << std::endl;
        std::cout << "  A: " << chunks1.size() << std::endl;
        std::cout << "  B: " << chunks2.size() << std::endl;
        std::cout << "  C: " << chunks3.size() << std::endl;
        std::cout << "  D: " << chunks4.size() << std::endl;

        std::thread t1(producer, 1, 3, chunks1);
        std::thread t2(producer, 2, 5, chunks2);
        std::thread t3(producer, 3, 7, chunks3);
        std::thread t4(producer, 4, 4, chunks4);

        t1.join();
        t2.join();
        t3.join();
        t4.join();

    }

    return 0;
}
