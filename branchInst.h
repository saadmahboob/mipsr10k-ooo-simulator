/* 
 * File:   branchInst.h
 * Author: Sebastian
 *
 * Created on December 10, 2014, 12:14 PM
 */

#ifndef BRANCHINST_H
#define	BRANCHINST_H
#include "inst.h"

class branchInst : public inst {
public:
    branchInst(string op,int rs, int rt, int rd, int extra);
    branchInst(const branchInst& orig);
    virtual void printInstFormat();
    virtual int calc();
    virtual int edge();
    virtual ~branchInst();
private:

};

#endif	/* BRANCHINST_H */

