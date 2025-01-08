#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include "cache.h"


using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;

CacheLine::CacheLine() : valid(false), dirty(false), tag(-1) {}

Cache::Cache(int sizeLog2, int blockSizeLog2, int associativityLog2, int cyc)
        : blockSize(1 << blockSizeLog2),
          associativity(1 << associativityLog2),
          accessTime(cyc), hits(0), misses(0) {
    int size = 1 << sizeLog2; // Convert from log2 to actual size
    numSets = size / (blockSize * associativity); // Calculate the number of sets
    sets.resize(numSets, std::vector<CacheLine>(associativity, CacheLine()));
    lruLists.resize(numSets); // Initialize LRU lists for each set
}

int Cache::getIndex(int address) {
    return (address / blockSize) % numSets;
}

int Cache::getTag(int address) {
    return address / (blockSize * numSets);
}


int Cache::hitMiss(int address) {
    int index = getIndex(address); // Get the cache index for address
    int tag = getTag(address); // Get the tag for address
    int wayAddress = -1;

    for (int i = 0; i < associativity; ++i) {
        CacheLine &line = sets[index][i];
        if (line.valid && line.tag == tag) { // Check if it's a hit
            wayAddress = i; // Record the index of this line within the set
            return wayAddress; // Return the way index where the hit was found
        }
    }
    return wayAddress;
}


CacheLineEviction Cache::addAddress(int address) {
    int index = getIndex(address); // Get the cache index for address
    int tag = getTag(address); // Get the tag for address
//    cout << "the address is" << address << endl;
//    cout << "the index is" << index << endl;
//    cout << "the tag is" << tag << endl;
    CacheLineEviction cacheline_to_evict = {-1, 0};
    // First, check for an invalid line to use
    CacheLine* lineToReplace = nullptr;
    for (int i = 0; i < associativity; ++i) {
        if (!sets[index][i].valid) { // Found an invalid line
            lineToReplace = &sets[index][i];
            //cout << "found an invalid line" << address << endl;
            break; // Use this line and exit loop
        }
    }

    // If no invalid lines found, use the LRU policy to find a line to evict
    if (lineToReplace == nullptr) {
        int lruLineIndex = lruLists[index].back(); // Get the least recently used tag
        // Find the line to replace based on lruLineIndex
        for (int i = 0; i < associativity; ++i) {
            if (sets[index][i].valid && sets[index][i].tag == lruLineIndex) {
                lineToReplace = &sets[index][i];
                cacheline_to_evict.Address = calculateAddress(index, lineToReplace->tag, blockSize, numSets);
                cacheline_to_evict.dirty = sets[index][i].dirty;
                break;
            }
        }
    }

    // Load new block into cache line
    lineToReplace->valid = true; // Mark the line as valid
    lineToReplace->dirty = false; // Clear dirty bit for the new data
    lineToReplace->tag = tag; // Set the new tag
    //updateLRUList(index, tag); // Update LRU list with the newly loaded tag
    //printCache();
    return cacheline_to_evict;
}


void Cache::updateLRUList(int index, int tag) {
    // Remove the tag if it exists in the LRU list
    lruLists[index].remove(tag);
    lruLists[index].push_front(tag); // Insert the new access at the front

    // If the LRU list exceeds the associativity, remove the least recently used item
    if (lruLists[index].size() > associativity) {
        lruLists[index].pop_back(); // Remove the least recently used tag
    }
}

double Cache::getHitRate() const {
    return (hits + misses == 0) ? 0.0 : static_cast<double>(hits) / (hits + misses);
}

double Cache::getMissRate() const {
    return (hits + misses == 0) ? 0.0 : static_cast<double>(misses) / (hits + misses);
}

int Cache::getHits() const {
    return hits;
}

int Cache::getMisses() const {
    return misses;
}

int Cache::getNumSets() const {
    return numSets; // Facilitate access to numSets
}

int Cache::getAssociativity() const {
    return associativity; // Facilitate access to associativity
}

void Cache::invalidateAddress(int address) {
    int tag = getTag(address);
    // Iterate over each set in L1 and find the line with the matching tag
    for (int i = 0; i < numSets; ++i) {
        for (int j = 0; j < associativity; ++j) {
            CacheLine &line = sets[i][j]; // Access cache lines
            if (line.valid && line.tag == tag) { // Check if valid and matching tag
                line.valid = false; // Invalidate the line
                // Also, remove the tag from  corresponding LRU list
                lruLists[i].remove(tag); // Refer to lruLists directly
                return; // Stop searching after invalidating the line
            }
        }
    }
}

void Cache::dirtyAddress(int address, int wayAddress) {
    int index = getIndex(address); // Get the cache index for address
    int tag = getTag(address); // Get the tag for address
    CacheLine &line = sets[index][wayAddress]; // Direct access
    if (line.valid && line.tag == tag) { // Additional safety check
        line.dirty = true; // Mark the line as dirty
    }
}

void Cache::printCache() const {
    for (int i = 0; i < numSets; ++i) {
        std::cout << "Set " << i << ": ";
        for (int j = 0; j < associativity; ++j) {
            // Reference to the current CacheLine
            const CacheLine& line = sets[i][j];

            // Print the tag if valid
            if (line.valid) {
                std::cout << "[Tag: " << std::setw(4) << line.tag
                          << ", Valid: " << (line.valid ? "1" : "0")
                          << ", Dirty: " << (line.dirty ? "1" : "0") << "] ";
            } else {
                std::cout << "[Tag: null, Valid: 0, Dirty: 0] "; // If invalid, show null
            }
        }

        // Show LRU state
        std::cout << "| LRU: ";
        for (const auto& tag : lruLists[i]) {
            std::cout << tag << " "; // Print the tags in the LRU list
        }

        std::cout << std::endl; // Move to the next line after finishing the set
    }
}

void Cache::updateLru(int address){
    int index = getIndex(address); // Get the cache index for address
    int tag = getTag(address); // Get the tag for address
    updateLRUList(index, tag);
    return;
}

void Cache::update_hit() {
    hits++;
    return;
}

void Cache::update_miss() {
    misses++;
    return;
}

bool Cache::isTagNotInLRUList(int address) {
    if (address == -1){
        return true;
    }
    int index = getIndex(address); // Get the cache index for the given address
    int tag = getTag(address); // Get the tag for the given address

    // Check LRU list for the specific set
    for (const int& lruTag : lruLists[index]) {
        if (lruTag == tag) {
            return false; // The tag is in the LRU list
        }
    }

    return true; // The tag is not in the LRU list
}

// Function to calculate the memory address
int Cache::calculateAddress(int index, int tag, int blockSize, int numSets) {
    // Since the offset is always 0, we are ignoring it.
    // Calculate the address using tag and index
    unsigned long address = (static_cast<unsigned long>(tag) << (static_cast<int>(log2(blockSize)) + static_cast<int>(log2(numSets)))) |
                            (static_cast<unsigned long>(index) << static_cast<int>(log2(blockSize)));

    return address; // Return the calculated address
}


void Cache::dirtyAddress(int address) {
    int index = getIndex(address); // Get the cache index for address
    int tag = getTag(address); // Get the tag for address

    // Iterate over each line in the appropriate set
    for (auto &line : sets[index]) {
        if (line.valid && line.tag == tag) { // Check if the line matches the address
            line.dirty = true; // Mark the line as dirty

            // Update the LRU list for this set
            //updateLRUList(index, tag); // Move this tag to the front as most recently used
            return; // Exit the function after marking the line
        }
    }

    std::cout << "Address not found in cache to mark as dirty: " << std::hex << address << std::endl;
}