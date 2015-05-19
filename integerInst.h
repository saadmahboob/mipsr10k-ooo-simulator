/* 
 * File:   integerInst.h
 * Author: Sebastian
 *
 * Created on December 7, 2014, 5:30 PM
 */

#ifndef INTEGERINST_H
#define	INTEGERINST_H
#include "inst.h"

class integerInst : public inst {
public:
    integerInst(string op,int rs, int rt, int rd, int extra);
    integerInst(const integerInst& orig);
    virtual void printInstFormat();
    virtual int calc();
    virtual int edge();
    virtual ~integerInst();
private:

};

#endif	/* INTEGERINST_H */

