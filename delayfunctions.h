/* 
 * File:   delayfunctions.h
 * Author: Cass
 *
 * Created on June 3, 2017, 10:36 PM
 */

#ifndef DELAYFUNCTIONS_H
#define	DELAYFUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

    void inline delay() {
        for(int i = 0; i < 16; i++) {
            NOP();
        }
    }

    void inline lDelay() {
        for(int j = 0; j < 5; j++) {
            delay();
        }
    }


#ifdef	__cplusplus
}
#endif

#endif	/* DELAYFUNCTIONS_H */

