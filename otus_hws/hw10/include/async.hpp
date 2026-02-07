#pragma once

#include <cstddef>

#include "parser.hpp"
#include "manager.hpp"

namespace bulk_parser {

void* connect(size_t block_size) {
    Manager& manager = Manager::instance(std::vector<std::shared_ptr<IBulkSink>>{
        std::make_shared<FileBulkSink>(),
        std::make_shared<ConsoleBulkSink>()
    });
    auto* parser = manager.create_parser(block_size);
    return static_cast<void*>(parser);
}

void receive(void* context, const char* buffer, size_t size) {
    Parser* parser_ptr = static_cast<Parser*>(context);
    parser_ptr->receive(buffer, size);
}

void disconnect(void* context) {
    Parser* parser_ptr = static_cast<Parser*>(context);
    parser_ptr->flush();
    Manager::instance().destroy_parser(parser_ptr);
}

} // namespace bulk_parser
