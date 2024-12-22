#ifndef CACHE_HPP
#define CACHE_HPP

#include <cstdint>
#include <vector>

class cache_c
{
public:
    cache_c(int sets, int ways)
        : lines(sets, std::vector<cache_line_s>(ways, {.tag = 0, .valid = 0}))
    {
    }
    cache_c(cache_c &&)                 = default;
    cache_c(const cache_c &)            = default;
    cache_c &operator=(cache_c &&)      = default;
    cache_c &operator=(const cache_c &) = default;
    ~cache_c();

    int process_read(uint32_t addr);
    int process_write(uint32_t addr);
    double get_hit_rate() const;
    double get_miss_rate() const;

    /*private:*/
    struct cache_line_s
    {
        uint64_t tag : 63;
        uint64_t valid : 1;
    };

    std::vector<std::vector<cache_line_s>> lines;

    // Cache is a list of size #sets.
    // Each list entry has #ways entries.
    //
    // When trying to read, use the set from the addr
    // to access the first level and search in the second
    // level to see if we get a hit.
    //
    //                    Address (64 bit)
    //  ----------------------------------------------------
    //  |           tag           |   set   |    offset    |
    //  ----------------------------------------------------
    //
    //
    //                    Cache Structure
    //  ----------------------------------------------------    <--------
    //  |           tag           |   block (not present)  |            |
    //  ----------------------------------------------------            |
    //                           ...                                 same set
    //  ----------------------------------------------------            |
    //  |           tag           |   block (not present)  |            |
    //  ----------------------------------------------------    <--------
    //  |                        ...                       |
    //  ----------------------------------------------------    <--------
    //  |           tag           |   block (not present)  |            |
    //  ----------------------------------------------------            |
    //                           ...                                 same set
    //  ----------------------------------------------------            |
    //  |           tag           |   block (not present)  |            |
    //  ----------------------------------------------------    <--------
};

cache_c::~cache_c() {}

#endif  // CACHE_HPP
