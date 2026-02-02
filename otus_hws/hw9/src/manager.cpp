#include "manager.hpp"
#include "parser.hpp"

Parser* Manager::create_parser(std::size_t bulk_size) {
    Parser* p = new Parser(bulk_size);
    return p;
}

void Manager::destroy_parser(Parser* p) {
    delete p;
}
