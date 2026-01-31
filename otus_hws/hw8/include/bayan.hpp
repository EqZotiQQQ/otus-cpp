#pragma once

#include "hasher.hpp"
#include "file_handler.hpp"

#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <string>
#include <ranges>
#include <format>

// bad styleguide but keep it as is 
namespace po = boost::program_options;
namespace fs = std::filesystem;

constexpr auto DEFAULT_BLOCK_SIZE = 4096;
constexpr auto DEFAULT_HASH_ALGORITHM = "md5";

enum class AvailableHashAlgos {
    MD5=0,
};


struct Hash {
    AvailableHashAlgos algo;
    std::shared_ptr<IBlockHasher> hash_impl;
};

struct Config {
    std::vector<fs::path> include_paths;
    std::vector<fs::path> exclude_paths;
    size_t max_depth;
    size_t min_size;
    size_t block_size;
    std::vector<std::string> case_insensetive_masks;
    Hash hash;
};

inline const char* to_string(AvailableHashAlgos h) {
    switch (h) {
        case AvailableHashAlgos::MD5:    return "MD5";
        default:           return "Unknown";
    }
}


void print_config(const Config& cfg, std::ostream& os = std::cout) {
    os << "Config {\n";

    os << "  include_paths: [\n";
    for (const auto& p : cfg.include_paths) {
        os << "    \"" << p.string() << "\",\n";
    }
    os << "  ]\n";

    os << "  exclude_paths: [\n";
    for (const auto& p : cfg.exclude_paths) {
        os << "    \"" << p.string() << "\",\n";
    }
    os << "  ]\n";

    os << "  max_depth: " << cfg.max_depth << "\n";
    os << "  min_size: " << cfg.min_size << "\n";
    os << "  block_size: " << cfg.block_size << "\n";

    os << "  case_insensetive_masks: [\n";
    for (const auto& m : cfg.case_insensetive_masks) {
        os << "    \"" << m << "\",\n";
    }
    os << "  ]\n";

    os << "  hash: " << to_string(cfg.hash.algo) << "\n";
    os << "}\n";
}

std::optional<Config> parse_cmd_line_arguments(int argc, char** argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("include", po::value<std::vector<std::string>>()->multitoken(), "provide list of paths")
    ("exclude", po::value<std::vector<std::string>>()->multitoken(), "provide list of paths to be excluded from search")
    ("depth", po::value<size_t>()->default_value(0), "duplicate search depth")
    ("min-size", po::value<size_t>()->default_value(1), "minimum file size to be analyzed")
    ("block-size", po::value<size_t>()->default_value(DEFAULT_BLOCK_SIZE), "batch to be read per time")
    ("hash", po::value<std::string>()->default_value(DEFAULT_HASH_ALGORITHM), "hash function: md5, ...") // todo make something better
    ("i", po::value<std::vector<std::string>>()->multitoken(), "case insensetive immune mask")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm); 

    auto print_help = []{
        std::cout << "Help msg" << std::endl;
    };

    if (vm.contains("help")) {
        print_help();
    } else { 
        auto file_exists_check = [](const fs::path& path){
            if (!std::filesystem::exists(path)) {
                throw std::runtime_error(std::format("Include path {} not found!", path.c_str()));
            }
        };
        Config config;
        if (vm.contains("include")) {
            for (const auto& p_str: vm["include"].as<std::vector<std::string>>()) {
                auto p = fs::path(p_str);
                file_exists_check(p);
                config.include_paths.push_back(p);
            }
        }

        if (vm.contains("exclude")) {
            for (const auto& p_str: vm["exclude"].as<std::vector<std::string>>()) {
                config.exclude_paths.push_back(p_str);
            }
        }

        if (vm.contains("i")) {
            for (std::string mask: vm["i"].as<std::vector<std::string>>()) {
                std::transform(mask.begin(), mask.end(), mask.begin(), [](unsigned char c){ return std::tolower(c); });
                config.case_insensetive_masks.push_back(mask);
            }
        }

        config.min_size = vm["min-size"].as<size_t>();
        config.block_size = vm["block-size"].as<size_t>();
        config.max_depth = vm["depth"].as<size_t>();

        if (vm["hash"].as<std::string>() == DEFAULT_HASH_ALGORITHM) {
            auto hasher = std::make_shared<Md5Hasher>();
            config.hash = Hash{AvailableHashAlgos::MD5, hasher};
        } else {
            throw std::runtime_error("Selected unsupported hash. Available hash funcs: md5");
        }

        return config;
    }
    return {};
}

std::vector<fs::path> collect_dir_content(const fs::path& dir, const Config& config, size_t current_depth) {
    std::cout << "Searching here... " << dir.c_str() << std::endl;
    std::vector<fs::path> subdir_paths;
    for (const auto& dir_object: fs::directory_iterator(dir)) {
        if (std::any_of(config.exclude_paths.begin(), config.exclude_paths.end(), [&dir_object](const std::string& exclude_item){return dir_object.path().string().contains(exclude_item);})) {
            // filtered out by excluede path
            continue;
        }

        if (std::filesystem::is_directory(dir_object) && current_depth != config.max_depth) {
            std::vector<fs::path> paths = collect_dir_content(dir_object, config, current_depth + 1);
            #ifdef __cpp_lib_containers_ranges
            paths.append_range(subpaths);
            #else
            subdir_paths.insert(subdir_paths.end(), paths.cbegin(), paths.cend());
            #endif
        } else {
            std::string file_extension = dir_object.path().extension().string();
            std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(), [](unsigned char c){ return std::tolower(c); });
            if (
                std::any_of(config.case_insensetive_masks.begin(), config.case_insensetive_masks.end(), 
                [&file_extension](
                    const std::string& mask
                ){
                    return file_extension.contains(mask);
                }) 
                && fs::file_size(dir_object) > config.min_size
            ) {
                // mask and size ok
                subdir_paths.push_back(dir_object);
            }
        }
    }
    return subdir_paths;
}

std::vector<fs::path> collect_files(const Config& config) { 
    std::vector<fs::path> paths;
    
    for (const auto& include_path: config.include_paths) {
        const auto subpaths = collect_dir_content(include_path, config, 0);
        #ifdef __cpp_lib_containers_ranges
        paths.append_range(subpaths);
        #else
        paths.insert(paths.end(), subpaths.cbegin(), subpaths.cend());
        #endif
    }

    return paths;
}


std::vector<std::vector<fs::path>> get_equal_size_files_groups(const std::vector<fs::path>& files) {
    std::unordered_map<size_t, std::vector<fs::path>> size_to_path_mapping;

    for (const auto& file: files) {
        size_to_path_mapping[fs::file_size(file)].push_back(file);
    }

    std::vector<std::vector<fs::path>> eq_size_groups;

    for (auto& [_, files] : size_to_path_mapping) {
        if (files.size() > 1) {
            eq_size_groups.push_back(std::move(files));
        }
    }

    return eq_size_groups;
}

struct OpenFile {
    fs::path path;
    std::ifstream stream;
};

auto get_duplicated_files(const std::vector<fs::path>& files, const Config& config) {
    std::vector<std::vector<fs::path>> result;

    auto eq_size_groups = get_equal_size_files_groups(files);

    std::vector<char> buffer(config.block_size);
    FilePool file_pool;

    for (const auto& initial_group : eq_size_groups) {
        if (initial_group.size() < 2) {
            continue;
        }

        std::vector<std::vector<fs::path>> work_groups;
        work_groups.push_back(initial_group);

        size_t block_index = 0;

        while (!work_groups.empty()) {
            std::vector<std::vector<fs::path>> next_round_groups;

            for (const auto& group : work_groups) {
                if (group.size() < 2) {
                    continue;
                }

                std::unordered_map<std::string, std::vector<fs::path>> by_hash;

                for (const auto& path : group) {
                    auto& fh = file_pool.get(path);

                    fh.seek_block(block_index, config.block_size);
                    auto read_bytes = fh.read_block(buffer.data(), buffer.size());
                    if (read_bytes <= 0) {
                        continue;
                    }

                    std::string hash = config.hash.hash_impl->hash(buffer.data(), static_cast<size_t>(read_bytes));
                    by_hash[hash].push_back(path);
                }

                for (auto& [hash, same_hash_files] : by_hash) {
                    if (same_hash_files.size() > 1) {
                        next_round_groups.push_back(std::move(same_hash_files));
                    }
                }
            }

            if (next_round_groups.empty()) {
                break;
            }

            const auto file_size = fs::file_size(next_round_groups.front().front());
            std::uintmax_t next_offset = (block_index + 1) * config.block_size;

            if (next_offset >= file_size) {
                for (auto& g : next_round_groups) {
                    result.push_back(std::move(g));
                }
                break;
            }

            work_groups = std::move(next_round_groups);
            ++block_index;
        }
    }

    return result;
}


void print_dups(const std::vector<std::vector<fs::path>>& dup_groups) {
    for (const auto& group: dup_groups) {
        for (const auto& file: group) {
            std::cout << file << '\n';
        }
        std::cout << '\n';
    }

}
