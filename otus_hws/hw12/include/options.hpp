#pragma once

#include <cstdio>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>


using namespace boost;
namespace po = boost::program_options;

enum class Mode {
    MEAN=0,
    VAR
};

struct Options {
    Mode mode;
};

Options parse_options(int argc, char* argv[]) {
    bool help = false;
    std::string mode;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", po::bool_switch(&help), "show help message")
        ("mode,m", po::value<std::string>(&mode)->default_value("mean"),
         "mode: mean or var");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    Options opts{mode == "mean" ? Mode::MEAN : Mode::VAR};

    return opts;
}