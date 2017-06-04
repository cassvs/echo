/* 
 * File:   cecho.c
 * Author: Cass
 *
 * Created on May 4, 2017, 3:23 PM
 */
#define _XTAL_FREQ 64000000
#include <xc.h>

#include "configbits.h"

#include <stdio.h>
#include <stdlib.h>

#include "togglefunctions.h"
#include "delayfunctions.h"

/*
 * 
 */
#define addrhigh PORTA
#define addrlow PORTC
#define dataport PORTD

typedef union {
    unsigned short long address; //24 bits wide
    struct {
        unsigned char lobyte;
        unsigned char hibyte;
        //bit pagebit;
    };
} Address_t;

Address_t wptr, rptr;
unsigned char midpoint, knobcount, period, inbuffer, kbuffer;

#include "memoryfunctions.h"

void knob() {
    kEnable();
    lDelay(); //knob device runs slow; wait for it ti catch up
    kbuffer = dataport;
    if((4-abs(kbuffer-period))<0){
        period = kbuffer;
    }
    kDisable();
}

void interrupt isr() {
    //Interrupt Service routine.
    //Everything important (after init) will happen in here.
    if(ADIF) {
        ADIF = 0; //Clear interrupt flag
        inbuffer = ADRESH; //Load sample into buffer
        GO = 1; //Get the ADC going again

        wptr.address++; //Increment write pointer

        //Read pointer = write pointer - offset
        rptr.lobyte = wptr.lobyte;
        rptr.hibyte = wptr.hibyte - period;

        memWrite(wptr, inbuffer); //Store sample

        CCPR2L = memRead(rptr); //Read delayed sample into PWM register

        knobcount++; //Increment knob counter
        if (knobcount == 0) {knob();}; //Get value from knob device if knob counter == 0
    }
}

int main(int argc, char** argv) {

    //Kick the oscillator up to 64 MHz
    OSCTUNE = 0b01110000;
    PLLEN = 1;

    //Disable nuisance features
    WDTCON = 0; //Watchdog timer off
    SLRCON = 0; //Slew-rate limiters off
    ANSEL = 0; //Analog pins off
    ANSELH = 0; //The rest of the analog pins off

    PORTD = 0; //Dataport is input for now
    TRISD = 1;

    PORTA = 0; //Addrports are output
    TRISA = 0;
    PORTC = 0;
    TRISC = 0;

    PORTB = 0;
    readDisable();
    writeDisable();
    kDisable();
    TRISB = 0b11110000;

    inputDisable(); //Disable input

    ADCON2 = 0b00101110; //Set up ADC
    ADCON1 = 0;
    TRISE2 = 1;
    ANS7 = 1;
    ADCON0 = 0b00011101;

    for(int i = 0; i < 8; i++){lDelay();}; //Wait for level to stablize

    //Get midpoint sample from ADC
    GO = 1;
    while(GO) {};
    midpoint = ADRESH;

    GIE = 1; //Interrupt setup
    PEIE = 1;
    ADIE = 1;

    TRISB3 = 1; //PWM setup
    PR2 = 0xff;
    CCP2CON = 0b00001100;
    TMR2IF = 0;
    TMR2ON = 1;
    while(!TMR2IF) {};
    TMR2IF = 0;
    TRISB3 = 0;

    wptr.address = 0;

    //Fill memory with midpoint values
    for(;wptr.address < 0x10000; wptr.address++) {
        memWrite(wptr, midpoint);
    }

    wptr.address = 0; //Zero write address

    inputEnable(); //Enable input

    rptr.address = 0; //Zero read address
    knobcount = 0; //Clear knob counter

    GO = 1; //Start ADC

    //Mainloop (does nothing):
    while(1){};

    return (EXIT_SUCCESS);
}

