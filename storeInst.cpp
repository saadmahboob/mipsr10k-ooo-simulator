/* 
 * File:   storeInst.cpp
 * Author: Sebastian
 * 
 * Created on December 10, 2014, 12:13 PM
 */

#include "storeInst.h"

storeInst::storeInst(string op,int rs, int rt, int rd, int extra) : inst(op,rs,rt,rd,extra) {
    executeLatency = 1;
    queueType = 'A';
}

storeInst::storeInst(const storeInst& orig) {
}

void storeInst::printInstFormat() {
    cout << //"ID: " << setw(4) << instID << " " <<
            op << " " << setfill('0') << setw(2) << hex << rs << " " <<
            setfill('0') << setw(2) << hex << rt << " " << 
            setfill('0') << setw(2) << hex << rd << " " <<
            setfill('0') << setw(8) << hex << extra;
}

int storeInst::calc() {
    switch (currentStageToBeCompleted) {
        case 'F':
            return 1;
            break;
        case 'D':
            if (isActiveListFull()) 
                return 0;
            else {
                removeFromFreeList(true);
                addActiveListEntry(true);
                inOrderLoads.push_back(this->instID);
                return 1;
            }
            break;
        case 'I':
            if (isAddressInstQueueFull())
                return 0;
            else {
                addToAddressInstQueue();
                return 1;
            }
            break;
        case 'A':
            if (nCurrentAddrExecutes >= nMaxAddrExecutes  || busyBitTable[physRs])
                return 0;
            //if (executeStep == 1)
            nCurrentAddrExecutes = nCurrentAddrExecutes + 1;
            addStoreAddrDependencies();
            return 1;
            break;
        case 'E':
            if (inOrderLoads.front() != this->instID) //load/store queue check, make sure we're doing this in order
                return 0;
            else {
                return 1;
            }
            break;
        case 'C':
            if (nCurrentCycleCommits < nMaxCommits) {
                nCurrentCycleCommits = nCurrentCycleCommits + 1;
                willBeCommitted = true;
                return 1;
            }
            else
                return 0;
            break;
        default:
            return 0;
            break;
    }
}
int storeInst::edge() {
    switch (currentStageToBeCompleted) {
        case 'F':
            currentStageToBeCompleted = 'D';
            break;
        case 'D':
            currentStageToBeCompleted = 'I';
            break;
        case 'I':
            currentStageToBeCompleted = 'A';
            break;
        case 'A':
            currentStageToBeCompleted = 'E';
            break;
        case 'E':
            if (executeStep == executeLatency) {
                currentStageToBeCompleted = 'C';       
                removeFromAddressInstQueue();
                inOrderLoads.pop_front();
                removeFromActiveList();
                restoreFreeList(true);
            }
            else
                executeStep = executeStep + 1;
            break;
        case 'C':
            committed = true;
            currentStageToBeCompleted = ' ';
            removeStoreAddrDependencies();
            break;
        default:
            return 0;
            break;
    }
    return 0;
}

storeInst::~storeInst() {
}

