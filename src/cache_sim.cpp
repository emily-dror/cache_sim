#include "arg_parser.hpp"
#include "logging.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char **argv)
{
    argument_parser_c parser("Cache Simulation");

    // General arguments
    parser.add_argument("--logging", "Logging type", false);

    // Cache specific arguments
    parser.add_argument("#1", "Path to the input file", true);
    parser.add_argument("--mem-cyc", "Number of memory cycles", true);
    parser.add_argument("--bsize", "Block size (log2)", true);
    parser.add_argument("--l1-size", "L1 cache size (log2)", true);
    parser.add_argument("--l1-assoc", "L1 associativity (log2(# of ways))", true);
    parser.add_argument("--l1-cyc", "L1 cache cycles", true);
    parser.add_argument("--l2-size", "L2 cache size (log2)", true);
    parser.add_argument("--l2-assoc", "L2 associativity (log2(# of ways))", true);
    parser.add_argument("--l2-cyc", "L2 cache cycles", true);
    parser.add_argument("--wr-alloc",
                        "Write Allocate policy (0: No Write Allocate, 1: Write Allocate)", true);

    // Parse command line arguments
    if (parser.parse(argc, argv)) {
        return 1;
    }

    // Initialize logger
    auto &logger = logger_c::get_instance();
    logger.set_log_level(parser.get("--logging"));

    // declare sim var to enable multiple sims
    //
    // parse command line
    //
    // Initialize sim's paramaters
    //
    // Run the simulation
    //
    //      input file --> next inst. --> pass to L1 --> hit/miss?
    //      --> if hit continue --> else pass to L2 --> hit/miss?
    //      --> if hit continue --> if miss wait for main memory
    //
    // Get sim's stats.

    if (argc < 19) {
        std::cerr << "Not enough arguments" << std::endl;
        return 0;
    }

    // Get input arguments

    // File
    // Assuming it is the first argument
    std::ifstream file(parser.get("#1"));  // input file stream
    std::string line;
    if (!file || !file.good()) {
        // File doesn't exist or some other error
        std::cerr << "File not found" << std::endl;
        return 0;
    }

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string address;
        char operation = 0;  // read (R) or write (W)
        if (!(ss >> operation >> address)) {
            // Operation appears in an Invalid format
            std::cout << "Command Format error" << std::endl;
            return 0;
        }

        // DEBUG - remove this line
        std::cout << "operation: " << operation;

        std::string cutAddress = address.substr(2);  // Removing the "0x" part of the address

        // DEBUG - remove this line
        std::cout << ", address (hex)" << cutAddress;

        unsigned long int num = 0;
        num                   = strtoul(cutAddress.c_str(), NULL, 16);

        // DEBUG - remove this line
        std::cout << " (dec) " << num << std::endl;
    }

    double L1MissRate = 0.0;
    double L2MissRate = 0.0;
    double avgAccTime = 0.0;

    printf("L1miss=%.03f ", L1MissRate);
    printf("L2miss=%.03f ", L2MissRate);
    printf("AccTimeAvg=%.03f\n", avgAccTime);

    return 0;
}
