/* 
 * File:   branchInst.cpp
 * Author: Sebastian
 * 
 * Created on December 10, 2014, 12:14 PM
 */

#include "branchInst.h"

branchInst::branchInst(string op,int rs, int rt, int rd, int extra) : inst(op,rs,rt,rd,extra) {
    executeLatency = 1;
}

branchInst::branchInst(const branchInst& orig) {
}

void branchInst::printInstFormat() {
    cout << //"ID: " << setw(4) << instID << " " <<
            op << " " << setfill('0') << setw(2) << hex << rs << " " <<
            setfill('0') << setw(2) << hex << rt << " " << 
            setfill('0') << setw(2) << hex << rd << " " <<
            setfill('0') << setw(8) << hex << extra;
}

int branchInst::calc() {
    switch (currentStageToBeCompleted) {
        case 'F':
            return 1;
            break;
        case 'D':
            if (isActiveListFull()) 
                return 0;
            else {
                removeFromFreeList();
                addActiveListEntry();
                addToBranchStack();
                currentBranch = this->instID;
                return 1;
            }
            break;
        case 'I':
            if (isIntegerInstQueueFull())
                return 0;
            else {
                addToIntegerInstQueue();
                return 1;
            }
            break;
        case 'E':
            if (executeStep == 1 && (nCurrentIntExecutes >= nMaxIntExecutes || nCurrentBranchExecutes == 1
                    || busyBitTable[physRs] || busyBitTable[physRt]))
                return 0;
            else {
                if (executeStep == 1) {
                    nCurrentIntExecutes = nCurrentIntExecutes + 1;
                    nCurrentBranchExecutes = nCurrentBranchExecutes + 1;
                }
                return 1;
            }
            break;
        case 'C':
            if (nCurrentCycleCommits < nMaxCommits) {
                willBeCommitted = true;
                nCurrentCycleCommits = nCurrentCycleCommits + 1;
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
int branchInst::edge() {
    switch (currentStageToBeCompleted) {
        case 'F':
            currentStageToBeCompleted = 'D';
            break;
        case 'D':
            currentStageToBeCompleted = 'I';
            break;
        case 'I':
            currentStageToBeCompleted = 'E';
            break;
        case 'E':
            if (executeStep == executeLatency) {
                currentStageToBeCompleted = 'C';       
                removeFromActiveList();
                restoreFreeList();
                removeFromIntegerInstQueue();
                removeFromBranchStack();
                if (extra == 1) { //mispredict
                    mispredictBranch = this->instID;
                    mispredictFire = true;
                    this->extra = 0; //set to no mispredict for redo
                }
            }
            else
                executeStep = executeStep + 1;
            break;
        case 'C':
            committed = true;
            currentStageToBeCompleted = ' ';
            break;
        default:
            return 0;
            break;
    }
    return 0;
}

branchInst::~branchInst() {
}

