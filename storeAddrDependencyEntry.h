/* 
 * File:   storeAddrDependencyEntry.h
 * Author: Sebastian
 *
 * Created on December 19, 2014, 1:57 AM
 */

#ifndef STOREADDRDEPENDENCYENTRY_H
#define	STOREADDRDEPENDENCYENTRY_H

class storeAddrDependencyEntry {
public:
    storeAddrDependencyEntry();
    storeAddrDependencyEntry(int id, int addr);
    storeAddrDependencyEntry(const storeAddrDependencyEntry& orig);
    virtual ~storeAddrDependencyEntry();
    int id;
    int addr;
private:

};

#endif	/* STOREADDRDEPENDENCYENTRY_H */

