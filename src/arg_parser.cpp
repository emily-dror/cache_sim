#include "arg_parser.hpp"

#include "logging.hpp"

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

int argument_parser_c::parse(int argc, char *argv[])
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

    //  log command line arguments
    LOG(DEBUG, "Input file: %s\n", get("#1").c_str());
    LOG(DEBUG, "Memory cycles: %s\n", get("--mem-cyc").c_str());
    LOG(DEBUG, "Block size: %s\n", get("--bsize").c_str());
    LOG(DEBUG, "Write Allocate policy: %s\n", get("--wr-alloc").c_str());
    LOG(DEBUG, "L1 size: %s\n", get("--l1-size").c_str());
    LOG(DEBUG, "L1 associativity: %s\n", get("--l1-assoc").c_str());
    LOG(DEBUG, "L1 cycles: %s\n", get("--l1-cyc").c_str());
    LOG(DEBUG, "L2 size: %s\n", get("--l2-size").c_str());
    LOG(DEBUG, "L2 associativity: %s\n", get("--l2-assoc").c_str());
    LOG(DEBUG, "L2 cycles: %s\n", get("--l2-cyc").c_str());

    // Check required arguments
    for (const auto &entry : arguments) {
        if (entry.second.required && entry.second.value.empty()) {
            std::cerr << "Missing required argument: " << entry.first << "\n";
            print_help();
            return 1;
        }
    }
    return 0;
}

std::string argument_parser_c::get(const std::string &name) const
{
    if (arguments.find(name) == arguments.end()) {
        return "(null)";
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
