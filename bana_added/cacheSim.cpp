#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include "cache.h"


using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;


int main(int argc, char **argv) {

    if (argc < 19) {
        cerr << "Not enough arguments" << endl;
        return 0;
    }

    // Get input arguments

    // File
    // Assuming it is the first argument
    char *fileString = argv[1];
    ifstream file(fileString); //input file stream
    string line;
    if (!file || !file.good()) {
        // File doesn't exist or some other error
        cerr << "File not found" << endl;
        return 0;
    }

//    std::ofstream fileOut("output.txt");
//    cout.rdbuf(fileOut.rdbuf());

    unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
            L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

    for (int i = 2; i < 19; i += 2) {
        string s(argv[i]);
        if (s == "--mem-cyc") {
            MemCyc = atoi(argv[i + 1]);
        } else if (s == "--bsize") {
            BSize = atoi(argv[i + 1]);
        } else if (s == "--l1-size") {
            L1Size = atoi(argv[i + 1]);
        } else if (s == "--l2-size") {
            L2Size = atoi(argv[i + 1]);
        } else if (s == "--l1-cyc") {
            L1Cyc = atoi(argv[i + 1]);
        } else if (s == "--l2-cyc") {
            L2Cyc = atoi(argv[i + 1]);
        } else if (s == "--l1-assoc") {
            L1Assoc = atoi(argv[i + 1]);
        } else if (s == "--l2-assoc") {
            L2Assoc = atoi(argv[i + 1]);
        } else if (s == "--wr-alloc") {
            WrAlloc = atoi(argv[i + 1]);
        } else {
            cerr << "Error in arguments" << endl;
            return 0;
        }
    }

    // Initialize L1 and L2 caches
    Cache L1Cache(L1Size, BSize, L1Assoc, L1Cyc);
    Cache L2Cache(L2Size, BSize, L2Assoc, L2Cyc);

    double totalAccessTime = 0.0; // Total access time accumulator
    int totalAccesses = 0; // Counter for total accesses

    while (getline(file, line)) {
//        cout << "------L1------" << endl;
//        L1Cache.printCache();
//        cout << "-----------" << endl;
//        L2Cache.printCache();
//        cout << "------L2------" << endl;
//        cout << line << endl;
        stringstream ss(line);
        string str_address;
        char operation = 0; // read (R) or write (W)
        if (!(ss >> operation >> str_address)) {
            // Operation appears in an Invalid format
            cout << "Command Format error" << endl;
            return 0;
        }

        totalAccesses++;
        string cutAddress = str_address.substr(2); // Removing the "0x" part of the address
        int address = 0;
        address = strtoul(cutAddress.c_str(), NULL, 16);
        bool isWrite = (operation == 'w'); // Determine if the operation is a write
        int wayAddress1 = L1Cache.hitMiss(address);
        int wayAddress2 = L2Cache.hitMiss(address);
        bool is_hitL1 = (wayAddress1 != -1);
        bool is_hitL2 = (wayAddress2 != -1);

        CacheLineEviction address_to_evict_L1 = {-1, 0};
        CacheLineEviction address_to_evict_L2 = {-1, 0};

        // insert address
        if (is_hitL1) {
            //cout << "Found in L1" << endl;
            totalAccessTime += L1Cyc;
            L1Cache.update_hit();
            if (isWrite) {
                L1Cache.updateLru(address);
                L1Cache.dirtyAddress(address);
                L1Cache.updateLru(address);
            } else {
                L1Cache.updateLru(address);
            }

        } else if (!is_hitL1 & is_hitL2) {
            //cout << "Found in L2 only" << endl;
            totalAccessTime += (L1Cyc + L2Cyc);
            L1Cache.update_miss();
            L2Cache.update_hit();
            if (isWrite) {
                if (WrAlloc) { // write + allocate
                    address_to_evict_L1 = L1Cache.addAddress(address);
                    L1Cache.dirtyAddress(address);
                    L1Cache.updateLru(address);
                    L2Cache.updateLru(address); //???
                } else { // write + no allocate
                    L2Cache.dirtyAddress(address);
                    L2Cache.updateLru(address);
                }
            } else {
                address_to_evict_L1 = L1Cache.addAddress(address);
                L1Cache.updateLru(address);
                L2Cache.updateLru(address); //????
            }


        } else if (!is_hitL1 & !is_hitL2) {
            //cout << "Found in MEM only" << endl;
            totalAccessTime += (L1Cyc + L2Cyc + MemCyc);
            L1Cache.update_miss();
            L2Cache.update_miss();
            if (isWrite) {
                if (WrAlloc) {
                    // write + allocate - dirty L1 + bring line to L1,2
                    address_to_evict_L1 = L1Cache.addAddress(address);
                    address_to_evict_L2 = L2Cache.addAddress(address);
                    L1Cache.dirtyAddress(address);
                    L1Cache.updateLru(address);
                    L2Cache.updateLru(address);
                } else {
                    // write + no allocate - do nothing, write to mem
                }
            } else {
                //cout << "reading" << endl;
                // read - bring address to both caches
                address_to_evict_L1 = L1Cache.addAddress(address);
                address_to_evict_L2 = L2Cache.addAddress(address);
                L1Cache.updateLru(address);
                L2Cache.updateLru(address);
            }
        } else {
            //cout << "ERROR!!" << endl;
        }

//        cout << "address_to_evict_L1.Address" << endl;
//        cout << address_to_evict_L1.Address << endl;
//        cout << "address_to_evict_L2.Address" << endl;
//        cout << address_to_evict_L2.Address << endl;


        // when an address is in eviction then it was LRU and so
        // it's not supposed to be in the LRU after insertion - check!!
//        if (!L1Cache.isTagNotInLRUList(address_to_evict_L1.Address)) {
//            cout << "Tag to evict is in L1 lrulist" << endl;
//        }
//        if (!L2Cache.isTagNotInLRUList(address_to_evict_L2.Address)) {
//            cout << "Tag to evict is in L2 lrulist" << endl;
//        }


        // evictions
        if (address_to_evict_L1.Address != -1) {
            if (address_to_evict_L1.dirty) { //if we are evicting something from L1 it must be in L2
                L2Cache.dirtyAddress(address_to_evict_L1.Address);
                L2Cache.updateLru(address_to_evict_L1.Address); // update lru in L2
            }
            //remove lru from l1 - supposed to happen during insertion
            // we already placed a line so no need to invalidate
        }

        if (address_to_evict_L2.Address != -1) {
            // check if there is a case where eviction addresses in L1 and L2 are different if both not -1
            if (address_to_evict_L1.Address != -1 & address_to_evict_L1.Address != address_to_evict_L2.Address) {
                //cout << "adress to evict in L1 and L2 are different" << endl;
            }

            if (L1Cache.hitMiss(address_to_evict_L2.Address) != -1) { //address is in L1
                // if L1 dirty then need to write it to memory
                L1Cache.invalidateAddress(address_to_evict_L2.Address); // this func updates lru
            } else { // address not in L1
                // no invalidation needed because we replaced the line
                //update lru in L2 (remove from lru), also no need
            }
        }
    }

//    cout << "------L1------" << endl;
//    L1Cache.printCache();
//    cout << "-----------" << endl;
//    L2Cache.printCache();
//    cout << "------L2------" << endl;


    double avgAccessTime = (totalAccesses > 0) ? (totalAccessTime / totalAccesses) : 0.0;

// Assuming this is inside your final output section in main
    cout << std::fixed << std::setprecision(3); // Set format for floating-point output
    cout << "L1miss=" << L1Cache.getMissRate() << " ";
    cout << "L2miss=" << L2Cache.getMissRate() << " ";
    cout << "AccTimeAvg=" << avgAccessTime << endl;

    return 0; // Successful execution
}
