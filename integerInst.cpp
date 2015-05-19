/* 
 * File:   integerInst.cpp
 * Author: Sebastian
 * 
 * Created on December 7, 2014, 5:30 PM
 */

#include "integerInst.h"

integerInst::integerInst(string op,int rs, int rt, int rd, int extra) : inst(op,rs,rt,rd,extra) {
    executeLatency = 1;
    queueType = 'I';
}

integerInst::integerInst(const integerInst& orig) {
}

void integerInst::printInstFormat() {
    cout << //"ID: " << setfill(' ') << setw(4) << instID << " " << 
            op << " " << setfill('0') << setw(2) << hex << rs << " " <<
            setfill('0') << setw(2) << hex << rt << " " << 
            setfill('0') << setw(2) << hex << rd << " " << 
            setfill(' ') << setw(8) << " ";;
}

int integerInst::calc() {
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
            if (isIntegerInstQueueFull())
                return 0;
            else {
                addToIntegerInstQueue();
                return 1;
            }
            break;
        case 'E':
            if (executeStep == 1 && (nCurrentIntExecutes >= nMaxIntExecutes  || busyBitTable[physRs] || busyBitTable[physRt]))
                return 0;
            else {
                if (executeStep == 1)
                    nCurrentIntExecutes = nCurrentIntExecutes + 1;
                return 1;
            }
            break;
        case 'W':
            return 1;
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
int integerInst::edge() {
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
                removeFromIntegerInstQueue();
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
    

integerInst::~integerInst() {
}


