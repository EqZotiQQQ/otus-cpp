#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <spdlog/spdlog.h>


struct JoinRow {
    int id;
    std::string a;
    std::string b;

    bool operator==(const JoinRow& other) const = default;
};

inline std::ostream& operator<<(std::ostream& os, const JoinRow& r) {
    os << "JoinRow{id=" << r.id
    << ", a=\"" << r.a
    << "\", b=\"" << r.b
    << "\"}";
    return os;
}

using JoinResult = std::vector<JoinRow>;

class JoinStorage {
public:
    std::string handle_command(const std::string& line) {
        spdlog::debug("Added new command: {}", line);
        std::string cmd = trim(line);
        if (cmd.empty()) {
            spdlog::debug("ERR empty command");
            return "ERR empty command\n";
        }

        auto tokens = split(cmd, ' ');
        if (tokens.empty()) {
            spdlog::debug("ERR empty command");
            return "ERR empty command\n";
        }

        const std::string& op = tokens[0];

        if (op == "INSERT") {
            return cmd_insert(tokens);
        } else if (op == "TRUNCATE") {
            return cmd_truncate(tokens);
        } else if (op == "INTERSECTION") {
            if (tokens.size() != 1) return "ERR invalid arguments\n";
            auto res = intersection();
            return format_result(res);
        } else if (op == "SYMMETRIC_DIFFERENCE") {
            if (tokens.size() != 1) return "ERR invalid arguments\n";
            auto res = symmetric_difference();
            return format_result(res);
        } else {
            spdlog::debug("ERR unknown command");
            return "ERR unknown command\n";
        }
    }


    JoinResult intersection() const {
        JoinResult out;
        for (const auto& [id, nameA] : A_) {
            auto it = B_.find(id);
            if (it != B_.end()) {
                out.push_back(JoinRow{id, nameA, it->second});
            }
        }
        return out;
    }

    JoinResult symmetric_difference() const {
        JoinResult out;

        for (const auto& [id, nameA] : A_) {
            if (B_.find(id) == B_.end()) {
                out.push_back(JoinRow{id, nameA, ""});
            }
        }

        for (const auto& [id, nameB] : B_) {
            if (A_.find(id) == A_.end()) {
                out.push_back(JoinRow{id, "", nameB});
            }
        }

        return out;
    }

    JoinResult get_table_content(const std::string& name) {
        JoinResult out;
        auto table = get_table(name);
        for (const auto& [id, nameA] : *table) {
            auto it = B_.find(id);
            if (it != B_.end()) {
                out.push_back(JoinRow{id, nameA, it->second});
            }
        }
        return out;
    }

private:
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \r\n\t");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \r\n\t");
        return s.substr(start, end - start + 1);
    }
    
    static std::vector<std::string> split(const std::string& s, char delim) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream iss(s);
        while (std::getline(iss, token, delim)) {
            tokens.push_back(token);
        }
        return tokens;
    }


    std::string cmd_insert(const std::vector<std::string>& tokens) {
        if (tokens.size() != 4) {
            spdlog::debug("ERR invalid arguments");
            return "ERR invalid arguments\n";
        }

        const std::string& table_name = tokens[1];
        const std::string& unique_id = tokens[2];
        const std::string& table_value = tokens[3];

        int id = 0;
        try {
            size_t pos = 0;
            id = std::stoi(unique_id, &pos);
            if (pos != unique_id.size()) {
                spdlog::debug("ERR invalid id");
                return "ERR invalid id\n";
            }
        } catch (...) {
            spdlog::debug("ERR invalid id");
            return "ERR invalid id\n";
        }

        auto* tbl = get_table(table_name);
        if (!tbl) {
            spdlog::debug("ERR unknown table: {}", table_name);
            return "ERR unknown table\n";
        }

        if (tbl->count(id) != 0) {
            spdlog::debug("ERR duplicate {}", id);
            return "ERR duplicate " + std::to_string(id) + "\n";
        }

        (*tbl)[id] = table_value;
        return "OK\n";
    }

    std::string cmd_truncate(const std::vector<std::string>& tokens) {
        if (tokens.size() != 2) {
            spdlog::debug("ERR invalid arguments");
            return "ERR invalid arguments\n";
        }

        const std::string& table = tokens[1];
        auto* tbl = get_table(table);
        if (!tbl) {
            spdlog::debug("ERR unknown table");
            return "ERR unknown table\n";
        }

        tbl->clear();
        spdlog::debug("Trancate successfully");
        return "OK\n";
    }
    

    std::map<int, std::string>* get_table(const std::string& name) {
        if (name == "A") return &A_;
        if (name == "B") return &B_;
        return nullptr;
    }

    static std::string format_result(const JoinResult& res) {
        std::ostringstream out;
        for (const auto& row : res) {
            out << row.id << "," << row.a << "," << row.b << "\n";
        }
        out << "OK\n";
        return out.str();
    }
private:
    std::map<int, std::string> A_;
    std::map<int, std::string> B_;
};
