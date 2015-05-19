/* 
 * File:   activeListEntry.cpp
 * Author: Sebastian
 * 
 * Created on December 13, 2014, 10:28 PM
 */

#include "activeListEntry.h"

activeListEntry::activeListEntry() {
}

activeListEntry::activeListEntry(int addr, int lReg, int pReg) {
    this->id = addr;
    this->destReg = lReg;
    this->oldDestReg = pReg; // get from register mapping
}

activeListEntry::activeListEntry(const activeListEntry& orig) {
}

activeListEntry::~activeListEntry() {
}

