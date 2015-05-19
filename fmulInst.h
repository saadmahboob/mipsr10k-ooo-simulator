/* 
 * File:   fmulInst.h
 * Author: Sebastian
 *
 * Created on December 10, 2014, 12:00 PM
 */

#ifndef FMULINST_H
#define	FMULINST_H
#include "inst.h"

class fmulInst : public inst {
public:
    fmulInst(string op,int rs, int rt, int rd, int extra);
    fmulInst(const fmulInst& orig);
    virtual void printInstFormat();
    virtual int calc();
    virtual int edge();
    virtual ~fmulInst();
private:

};

#endif	/* FMULINST_H */

