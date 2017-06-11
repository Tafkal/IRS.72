/**
 * @file main.c
 * @author Stefan Vukcevic
 * @date 2017
 * @brief Servo motor, project No. 72
 * Project designed the servo motor control from PC via UART and uC MSP430F5438A.
 */

/* Hardware includes */
#include <msp430.h> 
#include "servo_ctrl.h"

extern int fin;
extern unsigned char ascii_input[4];

/**
 * @brief main function
 */
int main (void){

    hw_init();

    UCA0TXBUF = 'I';

    while(1) {
        if (fin == 1) {
            servo_update();
        }
    }

}
