
#include "command_parser.hpp"
#include "coroutine_command_parser.hpp"

#include <sstream>


int main() {
    // { 
    //     std::cout << "Default approach\n####\n\n";
    //     CommandLineParser parser;
    //     std::stringstream mock_text;
    //     mock_text << "cmd1\ncmd2\ncmd3\n\ncmd4\ncmd5\n";
    //     parser.parse_command(mock_text);
    // }
    // {
    //     std::cout << "\n\n####\n\n";
    //     CommandLineParser parser;
    //     std::stringstream mock_text;
    //     mock_text << "cmd1\ncmd2\n{\ncmd3\ncmd4\n}\n{\ncmd5\ncmd6\n{\ncmd7\ncmd8\n}\ncmd9\n}\n{\ncmd10\ncmd11\n";
    //     parser.parse_command(mock_text);
    // }
    // {
    //     std::cout << "\n\n#### User input sequence has started. CTRL+D to stop recording\n\n";
    //     CommandLineParser parser;
    //     parser.parse_command(std::cin);
    // }


    { 
        std::cout << "Coroutine approach\n####\n\n";
        std::stringstream mock_text;
        mock_text << "cmd1\ncmd2\ncmd3\n\ncmd4\ncmd5\n";
        parse_stream(mock_text);
    }
    {
        std::cout << "\n\n####\n\n";
        std::stringstream mock_text;
        mock_text << "cmd1\ncmd2\n{\ncmd3\ncmd4\n}\n{\ncmd5\ncmd6\n{\ncmd7\ncmd8\n}\ncmd9\n}\n{\ncmd10\ncmd11\n";
        parse_stream(mock_text);
    }
    {
        std::cout << "\n\n#### User input sequence has started. CTRL+D to stop recording\n\n";
        parse_stream(std::cin);
    }

    return 0;
}
