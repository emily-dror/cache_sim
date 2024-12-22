#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <cstdint>
#include <string>

#define DEBUG (1 << 0)
#define L1_CACHE (1 << 1)
#define L2_CACHE (1 << 2)

class logger_c
{
public:
    logger_c(const char *name, uint8_t log_level);
    logger_c(const logger_c &)            = default;
    logger_c &operator=(const logger_c &) = default;
    ~logger_c();

    static uint8_t get_log_type(const std::string &arg);
    void proxy_log(uint8_t level, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

private:
    const char *name;
    uint8_t log_level;
};

#define LOG(TYPE, ...) logger.proxy_log(TYPE, "[" #TYPE "] " __VA_ARGS__)

#endif  // LOGGING_HPP
