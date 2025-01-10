#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <list>

// Struct to hold the result of handleMiss
struct CacheLineEviction {
    int Address;
    bool dirty;
};

class CacheLine {
public:
    bool valid;
    bool dirty;
    int tag;

    CacheLine();
};

class Cache {
private:
    int blockSize;
    int associativity;
    int numSets;
    std::vector<std::vector<CacheLine> > sets;
    int accessTime;
    int hits;
    int misses;
    std::vector<std::list<int> > lruLists;

    int getIndex(int address);
    int getTag(int address);

public:
    Cache(int sizeLog2, int blockSizeLog2, int associativityLog2, int cyc);

    // Cache management functions
    int hitMiss(int address); // Check for hit/miss and return way index
    CacheLineEviction addAddress(int address); // Add address to cache handling evictions
    void writeBack(int address); // Simulate write-back to memory
    void updateLRUList(int index, int tag); // Update LRU status for a set
    double getHitRate() const; // Get the hit rate
    double getMissRate() const; // Get the miss rate
    int getHits() const; // Get the number of hits
    int getMisses() const; // Get the number of misses
    int getNumSets() const; // Get the number of sets
    int getAssociativity() const; // Get the associativity
    void dirtyAddress(int address, int wayIndex); // Mark address as dirty for a specific way
    void invalidateAddress(int address); // Invalidate an address in the cache
    void updateLru(int address);
    bool isTagNotInLRUList(int address);
    void update_miss();
    void update_hit();
    int calculateAddress(int index, int tag, int blockSize, int numSets);
    void printCache() const;
    void dirtyAddress(int address);
};

#endif // CACHE_H
