#pragma once

#include <iostream>
#include <string>

#include <boost/asio/ip/address_v4.hpp>
#include <boost/program_options.hpp>


namespace po = boost::program_options;

struct Options {
    uint16_t port;
    boost::asio::ip::address_v4 ip_addr;
    uint8_t log_level;
    size_t history_depth_;
};


Options parse_options(int argc, char* argv[]) {
    Options opts;

    std::string ip_as_str;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "this message")
        ("history", po::value<size_t>(&opts.history_depth_)->default_value(100), "history size")
        ("port,p", po::value<uint16_t>(&opts.port)->default_value(12345), "port")
        ("log-level,l", po::value<uint8_t>(&opts.log_level)->default_value(1), "0-info+, 1-warn+")
        ("ip,i", po::value<std::string>(&ip_as_str)->default_value("127.0.0.1"), "ip address");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        std::exit(0);
    }

    boost::system::error_code ec;
    opts.ip_addr = boost::asio::ip::make_address_v4(ip_as_str, ec);
    if (ec) {
        throw std::runtime_error("Invalid IP address: " + ip_as_str + ". I expect something like 0-255.0-255.0-255.0-255");
    }

    return opts;
}
