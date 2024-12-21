#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <cstdint>
#include <string>

enum log_level_t
{
    DEBUG    = 1 << 0,
    L1_CACHE = 1 << 1,
    L2_CACHE = 1 << 2,
};

class logger_c
{
public:
    // Singleton
    static logger_c &get_instance();
    logger_c(const logger_c &)            = delete;
    logger_c &operator=(const logger_c &) = delete;
    ~logger_c();

    void set_log_level(const std::string &arg);
    void proxy_log(log_level_t level, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

private:
    logger_c();
    uint8_t log_level;
};

#define LOG(TYPE, ...) logger_c::get_instance().proxy_log(TYPE, "[" #TYPE "] " __VA_ARGS__)

#endif  // LOGGING_HPP
