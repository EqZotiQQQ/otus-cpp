#pragma once

#include <coroutine>
#include <iostream>
#include <vector>
#include <string>

/**

Команда поступает на вход
анализируем, что это команда
если всё ок - сохраняем
если это пробел или новый блок - извлекаем блок команд
блок команд пишется в файл
блок команд пишется в консоль
читаем дальше

*/

// struct ConsoleInputCorutineFrame {
//     struct PromiseType {
//         ConsoleInputCorutineFrame get_return_object() {
//             return {};
//         }
//         std::suspend_never initial_suspend() {
//             return {};
//         }
//         std::suspend_never final_suspend() noexcept {
//             return {};
//         }
//         void return_void() {}
//         void unhandled_exception() {
//             std::terminate();
//         }
//     };

//     using promise_type = PromiseType;

//     PromiseType promise_object;
//     std::vector<std::string> commands;
// };

// ConsoleInputCorutineFrame foo() {
//     co_return;
// }

struct CommandLineParser {
    void parse_command() {
        std::string command;
        while (std::getline(std::cin, command) || command == "\n") {
            command_decision(std::move(command));
        }
        if (depth_ == 0) {
            flush_commands();
        } else {
            std::cout << "Faced not closed brackets. Ignoring output" << std::endl;
        }
        std::cout << "EOF faced" << std::endl;
    }

    void parse_command(std::istream& input) {
        std::string command;
        while (std::getline(input, command) || command == "\n") {
            command_decision(std::move(command));
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
        stdout_stored_commands();
        commands_.clear();
    }

    void stdout_stored_commands() {
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

