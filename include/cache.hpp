#ifndef CACHE_HPP
#define CACHE_HPP

#include <cstdint>
#include <vector>

class cache_c
{
public:
    cache_c(uint8_t log, uint32_t size, uint32_t assoc, uint32_t blk);
    cache_c(cache_c &&)                 = default;
    cache_c(const cache_c &)            = default;
    cache_c &operator=(cache_c &&)      = default;
    cache_c &operator=(const cache_c &) = default;
    ~cache_c()                          = default;

    int process_access(char op, unsigned long addr);

private:
    struct cache_line_s
    {
        unsigned long tag;
        unsigned access;
        bool valid;
    };

    uint8_t log;
    uint32_t blk;
    uint32_t sets;
    std::vector<std::vector<cache_line_s>> lines;
};

#endif  // CACHE_HPP
