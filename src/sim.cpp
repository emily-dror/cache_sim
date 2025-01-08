#include "sim.hpp"

#include "logging.hpp"

#include <fstream>
#include <iostream>

sim_c::sim_c(const sim_config_s &config)
    : l1(L1_CACHE, config.l1_size, config.l1_assoc, config.blk_size, config.wr_alloc)
    , l2(L2_CACHE, config.l2_size, config.l2_assoc, config.blk_size, config.wr_alloc)
    , logger("SIM", config.log_type)
    , l1_cyc(config.l1_cyc)
    , l2_cyc(config.l2_cyc)
    , mem_cyc(config.mem_cyc)
{
}


sim_c::~sim_c() {}

void sim_c::run(const std::string &trace_path)
{
    std::ifstream tracer(trace_path);
    if (!tracer || !tracer.good()) {
        std::cerr << "File not found" << std::endl;
        return;
    }

    double l1_miss = 0, l1_access = 0, l2_miss = 0, l2_access = 0, total_access_time = 0;
    std::string line;
    while (getline(tracer, line)) {
        char op = line[0];
        if ((op != 'r' && op != 'w') || (line.substr(1, 3) != " 0x")) {
            std::cout << "Command Format error" << std::endl;
            return;
        }

        unsigned long addr = strtoul(line.substr(4).c_str(), NULL, 16);
        LOG(DEBUG, "op: %c address (hex) %lx (dec) %lu\n", op, addr, addr);

        ++l1_access;
        if (l1.process_access(op, addr)) {
            total_access_time += l1_cyc;
            continue;
        }

        ++l1_miss;
        ++l2_access;
        if (l2.process_access(op, addr)) {
            total_access_time += l2_cyc;
            continue;
        }

        ++l2_miss;
        total_access_time += l2_cyc + mem_cyc;
    }

    double L1MissRate = l1_access ? (l1_miss / l1_access) : 0.0,
           L2MissRate = l2_access ? (l2_miss / l2_access) : 0.0,
           avg_access_time = total_access_time / l1_access;
    printf("L1miss=%.03f L2miss=%.03f AccTimeAvg=%.03f\n", L1MissRate, L2MissRate, avg_access_time);

}
