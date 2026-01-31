#include "duplicate_searcher.hpp"

#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/program_options.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <string>
#include <format>
#include <ranges>

namespace po = boost::program_options;
namespace fs = std::filesystem;

constexpr auto DEFAULT_BLOCK_SIZE = 4096;
constexpr auto DEFAULT_HASH_ALGORITHM = "md5";

enum class Hash {
    MD5=0,
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
                auto p = fs::path(p_str);
                file_exists_check(p);
                config.exclude_paths.push_back(p);
            }
        }

        if (vm.contains("i")) {
            for (const auto& p: vm["i"].as<std::vector<std::string>>()) {
                config.case_insensetive_masks.push_back(p);
            }
        }

        config.min_size = vm["min-size"].as<size_t>();
        config.block_size = vm["block-size"].as<size_t>();
        config.max_depth = vm["depth"].as<size_t>();

        if (vm["hash"].as<std::string>() == DEFAULT_HASH_ALGORITHM) {
            config.hash = Hash::MD5;
        } else {
            throw std::runtime_error("Selected unsupported hash. Available hash funcs: md5");
        }

        return config;
    }
    return {};
}

std::vector<fs::path> collect_dir_content(const fs::path& dir, const Config& config, size_t current_depth) {
    std::vector<fs::path> subdir_paths;
    for (const auto& dir_object: fs::directory_iterator(dir)) {
        if (std::filesystem::is_directory(dir_object) && current_depth != config.max_depth) {
            std::vector<fs::path> paths = collect_dir_content(dir_object, config, current_depth + 1);
            #ifdef __cpp_lib_containers_ranges
            paths.append_range(subpaths);
            #else
            subdir_paths.insert(subdir_paths.end(), paths.cbegin(), paths.cend());
            #endif
        } else {
            std::string file_name = dir_object.path().stem().string();
            std::string file_extension = dir_object.path().extension().string();
            std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(), [](unsigned char c){ return std::tolower(c); });
            if (std::any_of(config.case_insensetive_masks.begin(), config.case_insensetive_masks.end(), [&file_extension](const std::string& mask){return file_extension.contains(mask);}) && fs::file_size(dir_object) > config.min_size) {
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
    for (const auto& [size, files]: size_to_path_mapping) {
        if (files.size() > 1) {
            eq_size_groups.push_back({});
            auto back = eq_size_groups.back();
            for (const auto& f: files) {
                back.push_back(f);
            }
        }
    }

    return eq_size_groups;
}

void get_duplicated_files(const std::vector<fs::path>& files, const Config& config) {
    std::vector<std::vector<fs::path>> eq_size_groups = get_equal_size_files_groups(files);

    std::unordered_map<size_t, std::vector<fs::path>> cached_files;
    std::string buffer(config.block_size, '\0');

    for (const auto& f_group: eq_size_groups) {
        std::ifstream file(f);
        if (!file) {
            throw std::runtime_error(std::format("Failed to open file {}. Aborting...", f.string()));
        }
        file.read(buffer.data(), config.block_size);
        
    }
}

int main(int argc, char** argv) {
    auto config = parse_cmd_line_arguments(argc, argv);
    if (config) {
        auto files = collect_files(*config);
        get_duplicated_files(files, *config);
    }


    return 0;
}
