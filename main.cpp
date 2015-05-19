/* 
 * File:   main.cpp
 * Author: Sebastian
 *
 * Created on December 6, 2014, 6:27 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "globals.h"
#include "inst.h"
#include "integerInst.h"
#include "faddInst.h"
#include "fmulInst.h"
#include "loadInst.h"
#include "storeInst.h"
#include "branchInst.h"

using namespace std;

bool quit = false;

int instructionID = 0; 
int nWayIssue = 4;
int nMaxCommits = 4;
int nCurrentCycleCommits = 0;
int nMaxIntExecutes = 2;
int nMaxFpAddExecutes = 1;
int nMaxFpMulExecutes = 1;
int nMaxAddrExecutes = 1;
int nCurrentIntExecutes = 0;
int nCurrentFpAddExecutes = 0;
int nCurrentFpMulExecutes = 0;
int nCurrentAddrExecutes = 0;
int nCurrentBranchExecutes = 0;
int currentFreeListIndex = 0;
int currentActiveListIndex = 0;
int lastCommitCycle = 0;
//int currentCycle = 0;


int nArchRegs = 32;
int nPhysRegs = 64;
map<int,int> regMap;

int nFreeListSize = 8;
deque< deque<int> > freeList;
int nActiveListSize = 8;
deque< deque<activeListEntry*> > activeList;

map<int,bool> busyBitTable;

int instQueueSize = 16;
deque<int> integerInstQueue;
deque<int> addressInstQueue;
deque<int> fpInstQueue;

deque<int> inOrderLoads;
deque<storeAddrDependencyEntry*> storeAddrDependencies;

deque<int> branchStack;
int currentBranch = 0;
bool mispredictFire = false;
int mispredictBranch = 0;


void printPipeline(deque< deque<char> > cycles, deque<inst*> instructionList, deque<int> instIndexBuffer ) {
    int cycleMaxEntryLength = cycles.back().size();
    for(int i = 0; i < cycles.size(); i++) {
        cycles[i].resize(cycleMaxEntryLength, ' ');
    }
    cout << setfill(' ') << setw(18) << "Instruction";
    for(int i = 1; i <= cycles.size(); i++) {
        cout << " " << setfill(' ') << setw(3) << i; 
    }
    cout << endl;
    for(int i = 0; i < cycleMaxEntryLength; i++) {
        //instructionList[instIndexBuffer[i]]->printInstFormat();
        instructionList[i]->printInstFormat();
        for(int j = 0; j < cycles.size(); j++) {
            cout << setfill(' ') << setw(3) << cycles[j][i] << " ";
        }
        cout << endl;
    }
}

void printInstInfo() {
    
}
/*
 * 
 */
int main(int argc, char** argv) {
    // Initialize globals further
    // reg map
    for(int i = 0; i < 32; i++) {
        regMap[i] = i;
    }
    // free list
    int fl1[] = {32,33,34,35,36,37,38,39};  deque<int> freeList1(&fl1[0], &fl1[0]+nFreeListSize);
    int fl2[] = {40,41,42,43,44,45,46,47};  deque<int> freeList2(&fl2[0], &fl2[0]+nFreeListSize); 
    int fl3[] = {48,49,50,51,52,53,54,55};  deque<int> freeList3(&fl3[0], &fl3[0]+nFreeListSize); 
    int fl4[] = {56,57,58,59,60,61,62,63};  deque<int> freeList4(&fl4[0], &fl4[0]+nFreeListSize);
    freeList.push_back(freeList1); freeList.push_back(freeList2); 
    freeList.push_back(freeList3); freeList.push_back(freeList4);
    // active list
    deque<activeListEntry*> activeList1; //activeList1.reserve(nActiveListSize); 
    deque<activeListEntry*> activeList2; //activeList2.reserve(nActiveListSize);
    deque<activeListEntry*> activeList3; //activeList3.reserve(nActiveListSize); 
    deque<activeListEntry*> activeList4; //activeList4.reserve(nActiveListSize);
    activeList.push_back(activeList1); activeList.push_back(activeList2); 
    activeList.push_back(activeList3); activeList.push_back(activeList4);  
    
    // Read trace file
    deque<inst*> instructionList;
    if(argc > 1) {
        ifstream traceFile(argv[1]);
        string traceLine;
        while(getline(traceFile, traceLine)) {
            istringstream iss(traceLine);
            string op; int rs = 0; int rt = 0; int rd = 0; long extra = 0;
            iss >> op;
            // integer/FPadd/FPmult format
            if(!op.compare("I") || !op.compare("A") || !op.compare("M")) {
                iss >> hex >> rs >> hex >> rt >> hex >> rd;
            }
            //load/store/branch format
            else if(!op.compare("L") || !op.compare("S")  || !op.compare("B")) {
                iss >> hex >> rs >> hex >> rt >> hex >> rd >> hex >> extra; 
            }
            inst *newInstruction = 0;
            if (!op.compare("I")) 
                newInstruction = new integerInst(op, rs, rt, rd, extra);
            else if(!op.compare("A"))
                newInstruction = new faddInst(op,rs,rt,rd,extra);
            else if(!op.compare("M"))
                newInstruction = new fmulInst(op,rs,rt,rd,extra);
            else if(!op.compare("L"))
                newInstruction = new loadInst(op,rs,rt,rd,extra);
            else if(!op.compare("S"))
                newInstruction = new storeInst(op,rs,rt,rd,extra);
            else if(!op.compare("B"))
                newInstruction = new branchInst(op,rs,rt,rd,extra);
            if (newInstruction) {
                instructionList.push_back(newInstruction);
            }
        }
    }
    //debugging: print what instructions we read from trace file
    //for (unsigned i=0; i<instructionList.size(); i++) {
    //    instructionList[i]->printInstFormat();
    //}
    
    deque< deque<char> > cycles;
    int currentCycle = 0;
    int fetchIterator = 0;
    deque<int> instIndexBuffer;
    deque<int> nonStalledInstIndexBuffer;
    deque<inst*> retainedInstInfo;
    while(!quit) {
        currentCycle++;
        // reset per cycle counter trackers
        nCurrentCycleCommits = 0;
        nCurrentIntExecutes = 0;
        nCurrentFpAddExecutes = 0;
        nCurrentFpMulExecutes = 0;
        nCurrentAddrExecutes = 0;
        nCurrentBranchExecutes = 0;
        currentActiveListIndex = 0;
        currentFreeListIndex = 0;
        
        quit = mispredictFire ? false : true; // set to false if there is any instance of us having to continue
        deque<char> cycleEntries;
        // continue ongoing instructions, add blank to cycle chart if already completed or stalled
        for(int i = 0; i < instIndexBuffer.size(); i++) {
            //continue ongoing instructions
            inst* currentInst = instructionList[instIndexBuffer[i]];
            if(!(currentInst->committed || currentInst->killed)) 
                quit = false;
            bool commitInOrder = true;
            int noStall;
            if(mispredictFire && currentInst->instID > mispredictBranch) {
                    noStall = true;
                    currentInst->killAndRestore();
             }
            else {
                if (currentInst->currentStageToBeCompleted == 'C') {
                    commitInOrder = currentInst->instID == 0 || 
                            (currentInst->instID > 0 && instructionList[currentInst->instID-1]->willBeCommitted);
                }
                noStall = commitInOrder ? currentInst->calc() : false;
            }
            if (noStall) {
                if (currentInst)
                cycleEntries.push_back(currentInst->currentStageToBeCompleted);
                //currentInst->edge();
                nonStalledInstIndexBuffer.push_back(instIndexBuffer[i]);
            }
            else //stall or committed
                cycleEntries.push_back(' ');
        }
        // Fetching new instructions
        int fetchSuccesses = 0;
        if(mispredictFire) {
            fetchIterator = mispredictBranch;
            //mispredictFire = false;
            //mispredictBranch = 0;
        }
        for (int i = 0; i < nWayIssue && fetchIterator+i < instructionList.size(); i++) {
            inst* toBeFetched = 0;
            if (mispredictFire) {
                toBeFetched = retainedInstInfo[fetchIterator+i];
                if (mispredictBranch == toBeFetched->instID) {
                    mispredictBranch = 0; 
                    toBeFetched->extra = 0;
                }
            }
            else
                toBeFetched = instructionList[fetchIterator+i];
            int canFetch = toBeFetched->calc();
            if(canFetch) {
                fetchSuccesses++;
                instIndexBuffer.push_back(toBeFetched->instID);
                cycleEntries.push_back(toBeFetched->currentStageToBeCompleted);
                nonStalledInstIndexBuffer.push_back(toBeFetched->instID);
                // copy to retain info in case of branch mispredicts
                inst* copyInst = 0;
                if (!toBeFetched->op.compare("I")) 
                    copyInst = new integerInst(toBeFetched->op,toBeFetched->rs,toBeFetched->rt,toBeFetched->rd,toBeFetched->extra);
                else if(!toBeFetched->op.compare("A"))
                    copyInst = new faddInst(toBeFetched->op,toBeFetched->rs,toBeFetched->rt,toBeFetched->rd,toBeFetched->extra);
                else if(!toBeFetched->op.compare("M"))
                    copyInst = new fmulInst(toBeFetched->op,toBeFetched->rs,toBeFetched->rt,toBeFetched->rd,toBeFetched->extra);
                else if(!toBeFetched->op.compare("L"))
                    copyInst = new loadInst(toBeFetched->op,toBeFetched->rs,toBeFetched->rt,toBeFetched->rd,toBeFetched->extra);
                else if(!toBeFetched->op.compare("S"))
                    copyInst = new storeInst(toBeFetched->op,toBeFetched->rs,toBeFetched->rt,toBeFetched->rd,toBeFetched->extra);
                else if(!toBeFetched->op.compare("B"))
                    copyInst = new branchInst(toBeFetched->op,toBeFetched->rs,toBeFetched->rt,toBeFetched->rd,toBeFetched->extra);
                if (copyInst) {
                    copyInst->instID = toBeFetched->instID;
                    retainedInstInfo.push_back(copyInst);
                }
                
                //toBeFetched->edge();
            }
            else {
                break;
            }
        }
        if(mispredictFire)
            mispredictFire = false;
        if(fetchSuccesses)
            quit = false;
        fetchIterator = fetchIterator+fetchSuccesses;
        for(int i = 0; i < nonStalledInstIndexBuffer.size(); i++) {
            //continue ongoing instructions
            inst* currentInst = instructionList[nonStalledInstIndexBuffer[i]];
            currentInst->edge();
        }
        nonStalledInstIndexBuffer.clear();
        cycles.push_back(cycleEntries);
    }
            
    printPipeline(cycles, instructionList, instIndexBuffer);
    //printPipeline(cycles, retainedInstInfo, instIndexBuffer);
    printInstInfo();
    return 0;
}



