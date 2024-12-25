#include "cache.hpp"

#include "logging.hpp"

#include <cstdint>

cache_c::cache_c(uint8_t log, uint32_t size, uint32_t assoc, uint32_t blk, uint8_t wr_alloc)
    : log(log)
    , blk(blk)
    , sets(size - assoc - blk)
    , wr_alloc(wr_alloc)
    , lines(1 << sets, std::vector<cache_line_s>((1 << assoc), {.tag = 0, .access = 0, .valid = 0}))
{
}


static unsigned long bits(unsigned hi, unsigned lo, unsigned long addr)
{
    uint32_t mask = ((1u << (hi - lo + 1)) - 1) << lo;
    return (addr & mask) >> lo;
}

int cache_c::process_access(char op, unsigned long addr)
{
    logger_c logger("", log);
    unsigned long tag = (addr >> blk) >> sets;
    unsigned long set = bits(blk + sets - 1, blk, addr);
    LOG(L1_CACHE, "accessing tag = 0x%lx, set = 0x%lx\n", tag, set);

    for (cache_line_s &line : lines[set]) {
        LOG(L1_CACHE, "checking tag = 0x%lx\n", line.tag);

        if (op == 'w' && !line.valid) {  // Write miss and line is invalid
            if (wr_alloc == 0) {         // No Write Allocate
                LOG(L1_CACHE, "No Write Allocate: Writing directly to memory\n");
                return 0;  // Miss, write directly to memory
            } else {       // Write Allocate enabled
                LOG(L1_CACHE, "Write Allocate: Loading block into cache\n");
                // Fall through to LRU eviction logic (we continue to handle cache eviction below)
            }
        }

        if (tag == line.tag && line.valid) {
            ++line.access;
            return 1;  // Cache hit
        }
    }

    // If it was a miss and write-allocate was enabled, we proceed with eviction
    unsigned evict = 0;
    for (unsigned i = 1; i < lines[set].size(); ++i) {
        if (lines[set][evict].access > lines[set][i].access) {
            evict = i;
        }
    }
    LOG(L1_CACHE, "evicting tag = 0x%lx\n", lines[set][evict].tag);
    lines[set][evict] = {
        .tag    = tag,
        .access = 1,
        .valid  = 1,
    };
    LOG(L1_CACHE, "populating tag = 0x%lx\n", lines[set][evict].tag);
    return 0;  // Cache miss, block loaded into cache
}


/*
int cache_c::process_access(char op, unsigned long addr)
{
    logger_c logger("", log);
    unsigned long tag = (addr >> blk) >> sets;
    unsigned long set = bits(blk + sets - 1, blk, addr);
    LOG(L1_CACHE, "accessing tag = 0x%lx, set = 0x%lx\n", tag, set);
    for (cache_line_s &line : lines[set]) {
        LOG(L1_CACHE, "checking tag = 0x%lx\n", line.tag);
        if (tag == line.tag && line.valid) {
            ++line.access;
            return 1;
        }
    }

    unsigned evict = 0;
    for (unsigned i = 1; i < lines[set].size(); ++i) {
        if (lines[set][evict].access > lines[set][i].access) {
            evict = i;
        }
    }
    LOG(L1_CACHE, "evicting tag = 0x%lx\n", lines[set][evict].tag);
    lines[set][evict] = {
        .tag    = tag,
        .access = 1,
        .valid  = 1,
    };
    LOG(L1_CACHE, "populating tag = 0x%lx\n", lines[set][evict].tag);
    return 0;
}
*/

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
