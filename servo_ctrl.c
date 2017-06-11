/**
 * @file servo_ctrl.c
 * @brief Implementation of a servo control system
 *
 * @date 2017
 * @author Stefan Vukcevic (vukca95@gmail.com)
 */

/* Hardware includes */
#include <msp430.h>
#include "servo_ctrl.h"

unsigned char ascii_input[4],
              i = 0;
int           pulse_w = 49,
              fin = 0;

/**
 * @brief Hardware initialization function
 * Initializes hardware components, a timer, PWM out on Port4 Bit1, configures UART
 * and enables interrupts.
 */
void hw_init() {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    //PWM
    P4DIR |= BIT1;              // P4.1 out
    P4SEL |= BIT1;              // P4.1 use for TB

    TB0CCTL1 = OUTMOD_7;        // reset/set outmode

    TB0CCR0 = 655;               // period TODO: SET THIS TO 20 MS
    TB0CCR1 = 49;                // initial pulse width value TODO: SET THIS TO 1.5 MS

    TB0CTL = TBSSEL__ACLK | MC__UP; // 32 kHz watch clk

    // configure UART
    P3SEL |= BIT4 | BIT5;       // configure P3.4 and P3.5 for uart

    UCA0CTL1 |= UCSWRST;        // enter sw reset

    UCA0CTL0 = 0;
    UCA0CTL1 = UCSSEL__ACLK | UCSWRST;
    UCA0BRW = 3;
    UCA0MCTL = UCBRS_3;         // configure 9600 bps

    UCA0CTL1 &= ~UCSWRST;       // leave sw reset

    UCA0IE |= UCRXIE;           // enable RX interrupt


    __enable_interrupt();
}

/**
 * @brief This is an ascii_to_digit and is_digit hybrid function.
 * If the ascii input character is a digit, the function returns that digit in int format,
 * else it returns the int value for 'F'.
 */
int to_digit(unsigned char ascii_input){
    int test = ascii_input - '0';
    if ((test >= 0) && (test <= 9)){
        return test;
    }
    else {
        return 'F';
    }
}

/**
 * @brief Takes an array of 3 chars and converts them to degrees for the servo turn.
 * The input is chars in the format of 'SignNumberNumber'. It is then converted to the int value,
 * to be sent to turn the servo.
 */
int ascii_to_deg(unsigned char ascii_input[3]){
    int deg,
        sgn = ascii_input[0],
        tens = to_digit(ascii_input[1]),
        ones = to_digit(ascii_input[2]);

    if ((ones != 'F') && (tens != 'F')){
        deg = (tens*10 + ones);
    }
    else{
        deg = 100;
    }
    if (sgn == '+'){
        return deg;
    }
    else if (sgn == '-'){
        return -deg;
    }
    else{
        return 100;
    }
}


/**
 * @brief This function updates the servo based on the UART input parsed by the ascii_got() function.
 * Here, timer controls the PWM for the servo.
 */
void servo_update(){
    int servo_deg;
    unsigned char ascii_deg[3] = {ascii_input[0], ascii_input[1], ascii_input[2]};
    if (ascii_input[0] == '+' || ascii_input[0] == '-'){
        servo_deg = ascii_to_deg(ascii_deg);
        if (servo_deg <= 90 && servo_deg >= -90){
            pulse_w = (int)(49 + (servo_deg*0.19)); // formula for the pulse width in respect of degrees.
            //TB0CTL &= MC__STOP;
            //TB0CTL |= TACLR;
            TB0CCR1 = pulse_w;
           // TB0CTL |= MC__UP;
        }
    } else{
        switch(ascii_input[0]){
        case 'a':
            pulse_w--;
            if (pulse_w < 32){
                pulse_w = 32;
            }
            //TB0CTL &= MC__STOP;
            //TB0CTL |= TACLR;
            TB0CCR1 = pulse_w;
            //TB0CTL |= MC__UP;
            break;
        case 'd':
            pulse_w++;
            if (pulse_w > 66){
                pulse_w = 66;
            }
           // TB0CTL &= MC__STOP;
            //TB0CTL |= TACLR;
            TB0CCR1 = pulse_w;
            //TB0CTL |= MC__UP;
            break;
        }
    }
    fin = 0;
}

/**
 * @brief This is the parser function for the UART receiver.
 * There are two ways to control this servo: by sending 'a' to turn left or 'd' to turn right,
 * or by sending a string in the form of 'Sign (+/-), Number, Number, 'd''.
 * Any mistake resets the communication and parsing.
 */
void ascii_got(unsigned char ascii_RX){

    switch (i){
    case 0:
        if (ascii_RX == 'a' || ascii_RX == 'd') {
            ascii_input[0] = ascii_RX;				// if the data sent is 'a' or 'd' it passes
            ascii_input[1] = '0';					// the value directly to the servo not waiting
            ascii_input[2] = '0';					// for other data.
            ascii_input[3] = '0';
            fin = 1;
        }
													// this part ensures that the sequence for the
        if (ascii_RX == '+' || ascii_RX == '-'){	// deg based parsing is respected.
            ascii_input[i] = ascii_RX;
            i++;
        }
        else{
            i = 0;
        }
        break;
    case 1:
    case 2:
        if (to_digit(ascii_RX) != 'F'){
            ascii_input[i] = ascii_RX;
            i++;
        }
        else{
            i = 0;
        }
        break;
    case 3:
        if (ascii_RX == 'd'){
            ascii_input[i] = ascii_RX;
            i = 0;
            fin = 1;
        }
        else{
            i = 0;
        }
        break;
    }
}
