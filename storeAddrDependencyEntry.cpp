/* 
 * File:   storeAddrDependencyEntry.cpp
 * Author: Sebastian
 * 
 * Created on December 19, 2014, 1:57 AM
 */

#include "storeAddrDependencyEntry.h"

storeAddrDependencyEntry::storeAddrDependencyEntry() {
}

storeAddrDependencyEntry::storeAddrDependencyEntry(int id, int addr) {
    this->id = id;
    this->addr = addr;
}

storeAddrDependencyEntry::storeAddrDependencyEntry(const storeAddrDependencyEntry& orig) {
}

storeAddrDependencyEntry::~storeAddrDependencyEntry() {
}

