/* 
 * File:   cecho.c
 * Author: Cass
 *
 * Created on May 4, 2017, 3:23 PM
 */
#define _XTAL_FREQ 64000000
#include <xc.h>
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H
#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 18        // Brown Out Reset Voltage bits (VBOR set to 1.8 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bit (WDT is controlled by SWDTEN bit of the WDTCON register)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC starts clocking the CPU without waiting for the oscillator to stablize.)
#pragma config MCLRE = OFF      // MCLR Pin Enable bit (RE3 input pin enabled; MCLR disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

#include <stdio.h>
#include <stdlib.h>

#include "togglefunctions.h"

/*
 * 
 */
#define addrhigh PORTA
#define addrlow PORTC
#define dataport PORTD

typedef union {
    unsigned int address;
    struct {
        unsigned char lobyte;
        unsigned char hibyte;
        //bit pagebit;
    };
} Address_t;

Address_t wptr, rptr;
unsigned char midpoint, knobcount, period, inbuffer, kbuffer;

unsigned char memRead(Address_t raddr) {
    unsigned char rvalue;
    addrlow = raddr.lobyte;
    addrhigh = raddr.hibyte;
    //Two NOPs go here
    readEnable();
    //Four NOPs go here
    rvalue = dataport;
    readDisable();
    return rvalue;
}

void memWrite(Address_t waddr, unsigned char data) {
    addrlow = waddr.lobyte;
    addrhigh = waddr.hibyte;
    //NOPNOP
    dataport = data;
    TRISD = 0;
    //NOPNOP
    writeEnable();
    //NOPNOP
    writeDisable();
    //NOPNOP
    TRISD = 255;
}

void knob() {
    kEnable();
    //long delay //knob device runs slow; wait for it ti catch up
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
    //Put mem into idle mode here
    //Disable knob device here
    TRISB = 0b11110000;

    inputDisable(); //Disable input

    ADCON2 = 0b00101110; //Set up ADC
    ADCON1 = 0;
    TRISE2 = 1;
    ANS7 = 1;
    ADCON0 = 0b00011101;

    //ldelay x 8 //Wait for level to stablize

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

