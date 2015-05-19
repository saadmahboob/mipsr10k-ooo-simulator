/* 
 * File:   activeListEntry.h
 * Author: Sebastian
 *
 * Created on December 13, 2014, 10:28 PM
 */

#ifndef ACTIVELISTENTRY_H
#define	ACTIVELISTENTRY_H

class activeListEntry {
public:
    activeListEntry();
    activeListEntry(int addr,int lReg, int pReg);
    activeListEntry(const activeListEntry& orig);
    int id;
    int oldDestReg;
    int destReg;
    virtual ~activeListEntry();
private:

};

#endif	/* ACTIVELISTENTRY_H */

