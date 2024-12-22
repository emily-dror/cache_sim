#include "arg_parser.hpp"
#include "logging.hpp"
#include "sim.hpp"

#include <cstdint>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc < 19) {
        std::cerr << "Not enough arguments" << std::endl;
        return 0;
    }

    argument_parser_c parser("Cache Simulation");
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
    parser.add_argument("--log-type", "Logging type", false);

    if (parser.parse(argc, argv)) {
        // Print help page and exit if parsing fails.
        return 1;
    }

#define stou(type, str) static_cast<type>(std::stoul(str))

    const sim_c::sim_config_s sim_config = {
        .mem_cyc  = stou(uint32_t, parser.get("--mem-cyc")),
        .blk_size = stou(uint32_t, parser.get("--bsize")),
        .l1_size  = stou(uint32_t, parser.get("--l1-size")),
        .l1_assoc = stou(uint32_t, parser.get("--l1-assoc")),
        .l1_cyc   = stou(uint32_t, parser.get("--l1-cyc")),
        .l2_size  = stou(uint32_t, parser.get("--l2-size")),
        .l2_assoc = stou(uint32_t, parser.get("--l2-assoc")),
        .l2_cyc   = stou(uint32_t, parser.get("--l2-cyc")),
        .wr_alloc = stou(uint8_t, parser.get("--wr-alloc")),
        .log_type = logger_c::get_log_type(parser.get("--log-type")),
    };

    sim_c sim(sim_config);

    sim.run(argv[1]);
    return 0;
}
