/* 
 * File:   loadInst.h
 * Author: Sebastian
 *
 * Created on December 10, 2014, 12:05 PM
 */

#ifndef LOADINST_H
#define	LOADINST_H
#include "inst.h"

class loadInst : public inst {
public:
    loadInst(string op,int rs, int rt, int rd, int extra);
    loadInst(const loadInst& orig);
    virtual void printInstFormat();
    virtual int calc();
    virtual int edge();
    virtual ~loadInst();
private:

};

#endif	/* LOADINST_H */

