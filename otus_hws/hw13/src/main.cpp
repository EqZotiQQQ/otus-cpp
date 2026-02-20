#include "models.hpp"

#include <iostream>
#include <iomanip>
#include <filesystem>

#include <boost/program_options.hpp>
#include <stdexcept>

namespace po = boost::program_options;
namespace fs = std::filesystem;

enum class AvailableModelTypes {
    LOGREG = 0,
    MLP
};

struct Options {
    fs::path dataset_path;
    AvailableModelTypes model_type;
    std::string model;
    std::optional<fs::path> w2_file;
};

Options parse_options(int argc, char* argv[]) {
    bool help = false;

    std::string model;
    std::string type;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", po::bool_switch(&help), "this message")
        ("file,f", po::value<std::string>(), "dataset")
        ("model,m", po::value<std::string>(&model), "Model file")
        ("type,t", po::value<std::string>(&type)->default_value("logreg"), "mode: logreg or mlp")
        ("w2", po::value<std::string>(), "MLP w2 weights");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    std::optional<fs::path> w2_file = vm.count("w2") ? std::make_optional(fs::path(model)) : std::nullopt;

    if (!vm.contains("file") || !fs::exists(vm["file"].as<std::string>())) {
        throw std::runtime_error("You should pass csv dataset --file {file_path}");
    }

    Options opts{
        fs::path(vm["file"].as<std::string>()),
        type == "logreg" ? AvailableModelTypes::LOGREG : AvailableModelTypes::MLP,
        model,
        w2_file
    };

    return opts;
}


int main(int argc, char* argv[]) {
    Options options = parse_options(argc, argv);

    std::unique_ptr<Model> model;
    if (options.model_type == AvailableModelTypes::LOGREG) {
        model = create_logreg_model(options.model);
    } else if (options.model_type == AvailableModelTypes::MLP && options.w2_file.has_value()) {
        model = create_mlp_model(options.model, *options.w2_file);
    } else {
        // ...
    }

    if (model) {
        double acc = model->compute_accuracy(options.dataset_path);
        spdlog::info("Got accuracy {}", acc);
    }

    return 0;
}