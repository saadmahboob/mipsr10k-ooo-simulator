/* 
 * File:   globals.h
 * Author: Sebastian
 *
 * Created on December 13, 2014, 6:29 PM
 */

#ifndef GLOBALS_H
#define	GLOBALS_H
#include <deque>
#include <map>
#include <algorithm>
#include "activeListEntry.h"
#include "storeAddrDependencyEntry.h"

using namespace std;

// might need to add last cycle with commit, so commits are done in order
extern int instructionID;   // Used to identify each instruction uniquely
extern int nWayIssue;       // n-way issue superscalar
extern int nMaxCommits;     // max commits per cycle
extern int nCurrentCycleCommits;
extern int nMaxIntExecutes; // 2 ALUS
extern int nMaxFpAddExecutes;  // 1 FP Add unit
extern int nMaxFpMulExecutes;  // 1 FP Mult unit
extern int nMaxAddrExecutes;// 1 load/store unit
extern int nCurrentIntExecutes;
extern int nCurrentFpAddExecutes;
extern int nCurrentFpMulExecutes;
extern int nCurrentAddrExecutes;
extern int nCurrentBranchExecutes;
extern int currentActiveListIndex;
extern int currentFreeListIndex;
extern int lastCommitCycle;

// Register map
extern int nArchRegs;       // n architectural registers defined by the ISA
extern int nPhysRegs;       // n physical registers implemented in the CPU
extern map<int,int> regMap; // architectural->physical register
// Free list(only 1 for this implementation, for both int and FP) contain lists of currently unassigned physical registers.
// Each list(our only list) consist of four parallel, eight-deep, circular FIFOs. 
extern int nFreeListSize;
extern deque< deque<int> > freeList;
// Active List (current instructions running) up to  32  instructions can be active, the 
// active list consists of four parallel, eight-deep, circular FIFOs.
extern int nActiveListSize;
extern deque< deque<activeListEntry*> > activeList;
// Busy bit table
// -For each physical register, the table contains a bit indicating whether the register currently contains a valid value
// -The tables sets a bit busy when the Corresponding register leaves the free list. It resets the bit 
//  when an execution unit writes  a  value into this register.
extern map<int,bool> busyBitTable;
// Reservation stations (instruction queues)
extern int instQueueSize;
extern deque<int> integerInstQueue;
extern deque<int> addressInstQueue;
extern deque<int> fpInstQueue;
// In-order Loads, store->load order
extern deque<int> inOrderLoads;
extern deque<storeAddrDependencyEntry*> storeAddrDependencies;
// Branch stack info
extern deque<int> branchStack;
extern int currentBranch;
extern bool mispredictFire;
extern int mispredictBranch;
// pipeline quit
extern bool quit;



#endif	/* GLOBALS_H */

