#include "logging.hpp"

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

logger_c::logger_c(const char *name, uint8_t log_level)
    : name(name)
    , log_level(log_level)
{
}

logger_c::~logger_c() {}

uint8_t logger_c::get_log_type(const std::string &arg)
{
    uint8_t retval = 0;
    if (arg.find("debug") != std::string::npos) {
        retval |= DEBUG;
    }

    if (arg.find("l1_cache") != std::string::npos) {
        retval |= L1_CACHE;
    }

    if (arg.find("l2_cache") != std::string::npos) {
        retval |= L2_CACHE;
    }
    return retval;
}

void logger_c::proxy_log(uint8_t level, const char *fmt, ...)
{
    if (!(level & log_level)) {
        return;
    }
    if (strlen(name)) {
        fprintf(stdout, "%s: ", name);
    }

    va_list arg;
    va_start(arg, fmt);
    vfprintf(stdout, fmt, arg);
    va_end(arg);
}
