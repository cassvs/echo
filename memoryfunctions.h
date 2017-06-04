/* 
 * File:   memoryfunctions.h
 * Author: Cass
 *
 * Created on June 3, 2017, 10:09 PM
 */

#ifndef MEMORYFUNCTIONS_H
#define	MEMORYFUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

unsigned char memRead(Address_t raddr) {
    unsigned char rvalue;
    addrlow = raddr.lobyte;
    addrhigh = raddr.hibyte;
    NOP();
    NOP();
    readEnable();
    NOP();
    NOP();
    NOP();
    NOP();
    rvalue = dataport;
    readDisable();
    return rvalue;
}

void memWrite(Address_t waddr, unsigned char data) {
    addrlow = waddr.lobyte;
    addrhigh = waddr.hibyte;
    NOP();
    NOP();
    dataport = data;
    TRISD = 0;
    NOP();
    NOP();
    writeEnable();
    NOP();
    NOP();
    writeDisable();
    NOP();
    NOP();
    TRISD = 0xff;
}



#ifdef	__cplusplus
}
#endif

#endif	/* MEMORYFUNCTIONS_H */

