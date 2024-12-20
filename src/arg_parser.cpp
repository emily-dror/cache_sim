#include "arg_parser.hpp"

#include <cstring>
#include <iostream>
#include <string>

argument_parser_c::argument_parser_c(const std::string &prog_name)
    : prog_name(prog_name)
{
}

void argument_parser_c::add_argument(const std::string &name, const std::string &description,
                                     bool required)
{
    arguments[name] = {description, "", required};
}

void argument_parser_c::parse(int argc, char *argv[])
{
    unsigned pos_arg = 1;
    for (int i = 1; i < argc; ++i) {
        if (!std::strncmp(argv[i], "--", 2)) {
            // named arguments
            arguments[argv[i]].value = argv[i + 1];
            ++i;
        } else {
            // positional arguments
            if (!is_present("#" + std::to_string(pos_arg))) {
                continue;
            }
            arguments["#" + std::to_string(pos_arg++)].value = argv[i];
        }
    }

    // Check required arguments
    for (const auto &entry : arguments) {
        if (entry.second.required && entry.second.value.empty()) {
            throw std::invalid_argument("Missing required argument: " + entry.first);
        }
    }
}

std::string argument_parser_c::get(const std::string &name) const
{
    if (arguments.find(name) == arguments.end()) {
        throw std::invalid_argument("Argument not found: " + name);
    }
    return arguments.at(name).value;
}

bool argument_parser_c::is_present(const std::string &name) const
{
    return arguments.find(name) != arguments.end();
}

void argument_parser_c::print_help() const
{
    std::cout << "Usage: " << prog_name << " [options]\n\n";
    for (const auto &entry : arguments) {
        std::cout << "  " << entry.first << "\t" << entry.second.description;
        if (entry.second.required) {
            std::cout << " (required)";
        }
        std::cout << "\n";
    }
}
