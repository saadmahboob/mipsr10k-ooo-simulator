/* 
 * File:   fmulInst.cpp
 * Author: Sebastian
 * 
 * Created on December 10, 2014, 12:00 PM
 */

#include "fmulInst.h"

fmulInst::fmulInst(string op,int rs, int rt, int rd, int extra) : inst(op,rs,rt,rd,extra){
    executeLatency = 3;
    queueType = 'F';
}


fmulInst::fmulInst(const fmulInst& orig) {
}

void fmulInst::printInstFormat() {
    cout << //"ID: " << setw(4) << instID << " " << 
            op << " " << setfill('0') << setw(2) << hex << rs << " " <<
            setfill('0') << setw(2) << hex << rt << " " << 
            setfill('0') << setw(2) << hex << rd << " " << 
            setfill(' ') << setw(8) << " ";
}

int fmulInst::calc() {
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
                return 1;
            }
            break;
        case 'I':
            if (isFpInstQueueFull())
                return 0;
            else {
                addToFpInstQueue();
                return 1;
            }
            break;
        case 'E':
            if (executeStep == 1 && (nCurrentFpAddExecutes >= nMaxFpAddExecutes  || busyBitTable[physRs] || busyBitTable[physRt]))
                return 0;
            else {
                if (executeStep == 1)
                    nCurrentFpAddExecutes = nCurrentFpAddExecutes + 1;
                return 1;
            }
            break;
        case 'W':
            return 1;
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
int fmulInst::edge() {
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
                currentStageToBeCompleted = 'W';       
                removeFromActiveList();
                restoreFreeList();
                removeFromFpInstQueue();
            }
            else
                executeStep = executeStep + 1;
            break;
        case 'W':
            currentStageToBeCompleted = 'C';
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

fmulInst::~fmulInst() {
}

