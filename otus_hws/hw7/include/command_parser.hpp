#pragma once

#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>


/**

Команда поступает на вход
анализируем, что это команда
если всё ок - сохраняем
если это пробел или новый блок - извлекаем блок команд
блок команд пишется в файл
блок команд пишется в консоль
читаем дальше

*/

struct CommandLineParser {
    void parse_command(std::istream& input) {
        std::string command;
        while (std::getline(input, command) || command == "\n") {
            command_decision(std::move(command));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (depth_ == 0) {
            flush_commands();
        } else {
            std::cout << "Faced not closed brackets. Ignoring output" << std::endl;
        }
        std::cout << "EOF faced" << std::endl;
    }

    void command_decision(std::string&& command) {
        if (command == "") {
            flush_commands();
            std::cout << "Block ended" << std::endl;
        } else if (command == "{") {
            if (depth_ == 0) {
                flush_commands();
                std::cout << "Dynamic block started" << std::endl;
            } else {
                std::cout << "Ignore inner block start" << std::endl;
            }
            depth_++;
        } else if (command == "}") {
            if (depth_ == 1) {
                flush_commands();
                std::cout << "Dynamic block ended" << std::endl;
            } else {
                std::cout << "Ignore inner block end" << std::endl;
            }
            depth_--;
        } else {
            store_command(std::move(command));
        }
    }

    void flush_commands() {
        std::cout << "Flush commands\n";
        if (commands_.empty()) {
            return;
        }
        stdout_stored_commands();
        dump_stored_commands_to_files();
        commands_.clear();
    }

    void dump_stored_commands_to_files() const {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);

        std::tm tm{};
        localtime_r(&t, &tm);

        std::ostringstream filename;
        filename << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".txt";

        std::ofstream file(filename.str());

        if (!file) {
            throw std::runtime_error("Failed to open file");
        }

        // file << "Zalupa" << std::endl;

        for (const std::string& line: commands_) {
            std::cout << "Write line " << line << std::endl;
            file << line << std::endl;
        }

    }

    void stdout_stored_commands() const {
        std::cout << "bulk: ";
        for (const auto& command: commands_) {
            std::cout << command << " ";
        }
        std::cout << std::endl;
    }

    void store_command(std::string command) {
        commands_.push_back(std::move(command));
    }

    int depth_ = 0;
    std::vector<std::string> commands_;
};

