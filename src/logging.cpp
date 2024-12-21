#include "logging.hpp"

#include <cstdarg>
#include <cstdio>

logger_c::logger_c()
    : log_level(0)
{
}

logger_c::~logger_c() {}

logger_c &logger_c::get_instance()
{
    static logger_c instance;
    return instance;
}

void logger_c::set_log_level(const std::string &arg)
{
    if (arg.find("debug") != std::string::npos) {
        log_level |= DEBUG;
    }

    if (arg.find("l1_cache") != std::string::npos) {
        log_level |= L1_CACHE;
    }

    if (arg.find("l2_cache") != std::string::npos) {
        log_level |= L2_CACHE;
    }
}

void logger_c::proxy_log(log_level_t level, const char *fmt, ...)
{
    if (!(level & log_level)) {
        return;
    }

    va_list arg;
    va_start(arg, fmt);
    vfprintf(stdout, fmt, arg);
    va_end(arg);
}
