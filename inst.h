/* 
 * File:   inst.h
 * Author: Sebastian
 *
 * Created on December 7, 2014, 5:31 PM
 */

#ifndef INST_H
#define	INST_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "globals.h"

using namespace std;
        
class inst {
public:
    inst(){};
    inst(string op,int rs, int rt, int rd, int extra);
    inst(const inst& orig);
    int assignID() { return instructionID++; };
    virtual void printInstFormat() = 0;
    virtual int calc() = 0;
    virtual int edge() = 0;
    virtual int isActiveListFull();
    virtual int addActiveListEntry(bool isLoad = false);
    virtual int removeFromActiveList();
    virtual int removeFromFreeList(bool isLoad = false);
    virtual int restoreFreeList(bool isLoad = false);
    virtual int isIntegerInstQueueFull();
    virtual int addToIntegerInstQueue();
    virtual int removeFromIntegerInstQueue();
    virtual int isAddressInstQueueFull();
    virtual int addToAddressInstQueue();
    virtual int removeFromAddressInstQueue();
    virtual int isFpInstQueueFull();
    virtual int addToFpInstQueue();
    virtual int removeFromFpInstQueue();
    virtual int addStoreAddrDependencies();
    virtual int removeStoreAddrDependencies();
    virtual int haveStoreAddrDependencies();
    virtual int isBranchStackFull();
    virtual int addToBranchStack();
    virtual int removeFromBranchStack();
    virtual int killAndRestore();
    virtual ~inst();
    string op;
    int rs;
    int rt;
    int rd;
    int extra;
    char currentStageToBeCompleted;
    int executeLatency;
    int executeStage;
    int executeStep;
    int instID;
    int physRs;
    int physRt;
    int physRd;
    bool committed;
    int activeListIndex;
    int freeListIndex;
    int oldPhysRd;
    int queueIndex;
    char queueType;
    int activeListNum;
    int branchMask;
    int memoryAddress;
    bool willBeCommitted;
    bool killed;
};

#endif	/* INST_H */

