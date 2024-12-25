#ifndef SIM_HPP
#define SIM_HPP

#include "cache.hpp"
#include "logging.hpp"

#include <cstdint>

class sim_c
{
public:
    struct sim_config_s
    {
        uint32_t mem_cyc;
        uint32_t blk_size;
        uint32_t l1_size;
        uint32_t l1_assoc;
        uint32_t l1_cyc;
        uint32_t l2_size;
        uint32_t l2_assoc;
        uint32_t l2_cyc;
        uint8_t wr_alloc;
        uint8_t log_type;
    };

    sim_c(const sim_config_s &config);
    ~sim_c();

    void run(const std::string &trace_path);

    // statistics (optional)
    void print_statistics();

private:
    const char *trace_file;
    cache_c l1;
    cache_c l2;
    logger_c logger;
    uint32_t l1_cyc;
    uint32_t l2_cyc;
    uint32_t mem_cyc;
};

#endif  // SIM_HPP
