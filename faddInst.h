/* 
 * File:   faddInst.h
 * Author: Sebastian
 *
 * Created on December 10, 2014, 11:36 AM
 */

#ifndef FADDINST_H
#define	FADDINST_H
#include "inst.h"

class faddInst : public inst {
public:
    faddInst(string op,int rs, int rt, int rd, int extra);
    faddInst(const faddInst& orig);
    virtual void printInstFormat();
    virtual int calc();
    virtual int edge();
    virtual ~faddInst();
private:

};

#endif	/* FADDINST_H */

