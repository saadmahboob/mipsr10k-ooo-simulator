/* 
 * File:   storeInst.h
 * Author: Sebastian
 *
 * Created on December 10, 2014, 12:13 PM
 */

#ifndef STOREINST_H
#define	STOREINST_H
#include "inst.h"

class storeInst : public inst {
public:
    storeInst(string op,int rs, int rt, int rd, int extra);
    storeInst(const storeInst& orig);
    virtual void printInstFormat();
    virtual int calc();
    virtual int edge();
    virtual ~storeInst();
private:

};

#endif	/* STOREINST_H */

