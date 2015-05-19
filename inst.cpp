/* 
 * File:   inst.cpp
 * Author: Sebastian
 * 
 * Created on December 7, 2014, 5:31 PM
 */

#include "inst.h"

inst::inst(string op,int rs, int rt, int rd, int extra) {
    this->op = op;
    this->rs = rs;
    this->rt = rt;
    this->rd = rd;
    this->extra = extra;
    this->currentStageToBeCompleted = 'F';
    this->executeStage = 0;
    this->executeStep = 1;
    this->executeLatency = 0;
    this->instID = assignID();
    this->physRs = 0;
    this->physRt = 0;
    this->physRd = 0;
    this->committed = false;
    this->oldPhysRd = 0;
    this->queueType = ' ';
    this->activeListNum = 0;
    this->branchMask = 0b0000;
    this->memoryAddress = 0x00000000;
    this->willBeCommitted = false;
    this->killed = false;
}

inst::inst(const inst& orig) {
}

int inst::isActiveListFull() {
    return activeList[0].size() == nActiveListSize && activeList[1].size() == nActiveListSize
                    && activeList[2].size() == nActiveListSize && activeList[3].size() == nActiveListSize;
}

int inst::addActiveListEntry(bool isLoad) {
    activeListEntry* entry = 0;
    if (isLoad) {
        entry = new activeListEntry(instID, rt, oldPhysRd);
    }
    else
        entry = new activeListEntry(instID, rd, oldPhysRd);
    //activeListIndex = currentActiveListIndex++; //might want to remember this value
    activeListIndex = 0;
    //if (activeListIndex >= 4)
    //    return 0; //error
    while (activeList[activeListIndex].size() == nActiveListSize)
        activeListIndex++;
        //activeListIndex = currentActiveListIndex++;
    //if (activeListIndex >= 4)
    //    return 0; //error
    activeList[activeListIndex].push_back(entry);
    for (int i = 0; i < activeList.size(); i++) {
        activeListNum += activeList[i].size();
    }
    activeListNum--;
    return 1;
}

int inst::removeFromActiveList() {
    activeListEntry* foundEntry;
    bool found = false;
    int index = 0;
    for (deque<activeListEntry*>::iterator it = activeList[activeListIndex].begin(); it!=activeList[activeListIndex].end(); ++it) {
        if ((*it)->id == instID) {
            foundEntry = *it;
            found = true;
            break;
        }
        index++;
    }
    if(!found)
        return 0;
    //regMap[rd] = foundEntry->oldDestReg;
    activeList[activeListIndex].erase(activeList[activeListIndex].begin()+index);
    delete foundEntry;
    return 1;
}

int inst::removeFromFreeList(bool isLoad) {
    /*if (rd == 0) {
        physRs = regMap[rs];
        physRt = regMap[rt];
        physRd = 0;
    }*/
    if (isLoad) {
        if (rt == 0) { // we never map this
            physRs = regMap[rs];
            physRt = 0;
            oldPhysRd = 0;
            physRd = 0;
            return 1;
        }
        //freeListIndex = currentFreeListIndex++; //might want to remember this value
        freeListIndex = 0;
        while (freeList[freeListIndex].size() == 0  && freeListIndex < 4)
            freeListIndex++;
        int newDest = freeList[freeListIndex].front();
        freeList[freeListIndex].pop_front();
        physRs = regMap[rs];
        physRt = newDest;
        physRd = 0;
        oldPhysRd = regMap[rt];
        regMap[rt] = physRt;
        busyBitTable[physRt] = true;
        return 1;
    }
    else {
        if (rd == 0) { // we never map this
            physRs = regMap[rs];
            physRt = regMap[rt];
            oldPhysRd = 0;
            physRd = 0;
            return 1;
        }
        freeListIndex = 0;
        while (freeList[freeListIndex].size() == 0  && freeListIndex < 4)
            freeListIndex++;
        int newDest = freeList[freeListIndex].front();
        freeList[freeListIndex].pop_front();
        physRs = regMap[rs];
        physRt = regMap[rt];
        physRd = newDest;
        oldPhysRd = regMap[rd];
        regMap[rd] = physRd;
        busyBitTable[physRd] = true;
        return 1;
    }
}

int inst::restoreFreeList(bool isLoad) {
    if (isLoad) {
        if (rt == 0)
            return 1;
        busyBitTable[physRt] = false;
        freeList[freeListIndex].push_back(physRt);
    }
    else {
        if (rd == 0)
            return 1;
        busyBitTable[physRd] = false;
        freeList[freeListIndex].push_back(physRd);
    }
    return 1;
}

int inst::isIntegerInstQueueFull() {
    return integerInstQueue.size() == instQueueSize;
}

int inst::addToIntegerInstQueue() {
    integerInstQueue.push_back(instID);
    queueIndex = integerInstQueue.size();
    return 1;
}

int inst::removeFromIntegerInstQueue() {
    integerInstQueue.pop_front();
    return 1;
}

int inst::isAddressInstQueueFull() {
    return addressInstQueue.size() == instQueueSize;
}

int inst::addToAddressInstQueue() {
    addressInstQueue.push_back(instID);
    queueIndex = addressInstQueue.size();
    return 1;
}

int inst::removeFromAddressInstQueue() {
    addressInstQueue.pop_front();
    return 1;
}

int inst::isFpInstQueueFull() {
    return fpInstQueue.size() == instQueueSize;
}

int inst::addToFpInstQueue() {
    fpInstQueue.push_back(instID);
    queueIndex = fpInstQueue.size();
    return 1;
}

int inst::removeFromFpInstQueue() {
    fpInstQueue.pop_front();
    return 1;
}

//storeAddrDependencies
int inst::addStoreAddrDependencies() {
    storeAddrDependencyEntry* entry = new storeAddrDependencyEntry(this->instID, this->extra);
    storeAddrDependencies.push_back(entry);
    return 1;
}

int inst::removeStoreAddrDependencies() {
    storeAddrDependencyEntry* foundEntry;
    int index = 0;
    bool found = false;
    for (deque<storeAddrDependencyEntry*>::iterator it = storeAddrDependencies.begin(); it!=storeAddrDependencies.end(); ++it) {
        if ((*it)->addr == this->extra) {
            foundEntry = *it;
            found = true;
            break;
        }
        index++;
    }
    if (!found) return 0;
    storeAddrDependencies.erase(storeAddrDependencies.begin()+index);   
    delete foundEntry;
    return 1;
}

int inst::haveStoreAddrDependencies() {
    bool found = false;
    for (deque<storeAddrDependencyEntry*>::iterator it = storeAddrDependencies.begin(); it!=storeAddrDependencies.end(); ++it) {
        if ((*it)->addr == this->extra && (*it)->id < instID) {
            found = true;
            break;
        }
    }
    if (!found) return 0;
    return 1;
    
}

int inst::isBranchStackFull() {
    if(branchStack.size() == 4)
        return 1;
    else
        return 0;
}

int inst::addToBranchStack() {
    branchStack.push_back(this->instID);
    return 1;
}

int inst::removeFromBranchStack() {
    //storeAddrDependencyEntry* foundEntry;
    int index = 0;
    bool found = false;
    for (deque<int>::iterator it = branchStack.begin(); it!=branchStack.end(); ++it) {
        if ((*it) == this->instID) {
            //foundEntry = *it;
            found = true;
            break;
        }
        index++;
    }
    if (!found) return 0;
    branchStack.erase(branchStack.begin()+index);   
    //delete foundEntry;
    return 1;
}

int inst::killAndRestore() {
    this->killed = true;
    if (!op.compare("I")) {
        switch(currentStageToBeCompleted) {
            case 'F': break;
            case 'D': break;
            case 'I':  
                removeFromActiveList();
                restoreFreeList();
                break;
            case 'E': 
                removeFromActiveList();
                restoreFreeList();
                removeFromIntegerInstQueue();
                break;
            case 'C': break;
            default: break;
        }
    }
    else if(!op.compare("A")) {
        switch(currentStageToBeCompleted) {
            case 'F': break;
            case 'D': break;
            case 'I':  
                removeFromActiveList();
                restoreFreeList();
                break;
            case 'E': 
                removeFromActiveList();
                restoreFreeList();
                removeFromFpInstQueue();
                break;
            case 'C': break;
            default: break;
        }
    }
    else if(!op.compare("M")) {
        switch(currentStageToBeCompleted) {
            case 'F': break;
            case 'D': break;
            case 'I': 
                removeFromActiveList();
                restoreFreeList();
                break;
            case 'E': 
                removeFromActiveList();
                restoreFreeList();
                removeFromFpInstQueue();
                break;
            case 'C': break;
            default: break;
        }
    }
    else if(!op.compare("L")) {
        switch(currentStageToBeCompleted) {
            case 'F': break;
            case 'D': break;
            case 'I':
                inOrderLoads.pop_front();
                removeFromActiveList();
                restoreFreeList();
                break;
            case 'A':
            case 'E': 
                removeFromAddressInstQueue();
                inOrderLoads.pop_front();
                removeFromActiveList();
                restoreFreeList(true);
                break;
            case 'C': break;
            default: break;
        }
    }
    else if(!op.compare("S")) {
        switch(currentStageToBeCompleted) {
            case 'F': break;
            case 'D': break;
            case 'I':
            case 'A':
                inOrderLoads.pop_front();
                removeFromActiveList();
                restoreFreeList();
                break;
            case 'E': 
                removeFromAddressInstQueue();
                inOrderLoads.pop_front();
                removeFromActiveList();
                restoreFreeList(true);
                removeStoreAddrDependencies();
                break;
            case 'C': break;
            default: break;
        }
    }
    else if(!op.compare("B")) {
        switch(currentStageToBeCompleted) {
            case 'F': break;
            case 'D': break;
            case 'I': 
                removeFromActiveList();
                restoreFreeList();
                removeFromBranchStack();
                break;
            case 'E': 
                removeFromActiveList();
                restoreFreeList();
                removeFromIntegerInstQueue();
                removeFromBranchStack();
                break;
            case 'C': break;
            default: break;
        }
    }
    else if(!op.compare("L")) {
        switch(currentStageToBeCompleted) {
            case 'F': break;
            case 'D': break;
            case 'I':
                inOrderLoads.pop_front();
                removeFromActiveList();
                restoreFreeList();
                break;
            case 'A':
            case 'E': 
                removeFromAddressInstQueue();
                inOrderLoads.pop_front();
                removeFromActiveList();
                restoreFreeList(true);
                break;
            case 'C': break;
            default: break;
        }
    }
    this->currentStageToBeCompleted = 'X';
    return 1;
}

inst::~inst() {
}

