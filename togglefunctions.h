/* 
 * File:   togglefunctions.h
 * Author: Cass
 *
 * Created on May 27, 2017, 9:01 PM
 */

#ifndef TOGGLEFUNCTIONS_H
#define	TOGGLEFUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

void inline readEnable() {
    RB1 = 0;
}

void inline readDisable() {
    RB1 = 1;
}

void inline writeEnable() {
    RB0 = 0;
}

void inline writeDisable() {
    RB0 = 1;
}

void inline kEnable() {
    RB2 = 1;
}

void inline kDisable() {
    RB2 = 0;
}

void inline inputEnable() {
    RB4 = 1;
}

void inline inputDisable() {
    RB4 = 0;
}

#ifdef	__cplusplus
}
#endif

#endif	/* TOGGLEFUNCTIONS_H */

