#ifndef SERVO_CTRL_H_
#define SERVO_CTRL_H_


extern void hw_init();
extern int to_digit(unsigned char ascii_input);
extern int ascii_to_deg(unsigned char ascii_input[3]);
extern void servo_update();
extern void ascii_got(unsigned char ascii_RX);

#endif /* SERVO_CTRL_H_ */
