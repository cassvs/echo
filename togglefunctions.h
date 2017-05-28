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

void readEnable() {
    RB1 = 0;
}

void readDisable() {
    RB1 = 1;
}

void writeEnable() {
    RB0 = 0;
}

void writeDisable() {
    RB0 = 1;
}

void kEnable() {
    RB2 = 1;
}

void kDisable() {
    RB2 = 0;
}

void inputEnable() {
    RB4 = 1;
}

void inputDisable() {
    RB4 = 0;
}

#ifdef	__cplusplus
}
#endif

#endif	/* TOGGLEFUNCTIONS_H */

