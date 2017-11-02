/*
 * scsm.h
 * Single Coil Machine Driver configuration file
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 */

#define PUSHBUTTON 10 // Push button pin
#define LEDCONTROL 13 // Pin for connection the controlling LED. By default used onboard Arduino LED.
// If no packet with an address has been received during this time, the decoder returns to the operating mode
#define DECODER_ADDRESS 1 // Default decoder DCC address
/*
 * Turnouts structures array. Mode details see on scsm.h file
 * {control_pin, enable_pin, red_pin, black_pin, pulse_time, motorRun}
 */
Turnout turnouts[]={{20,16,17,18,1000,0},{19,12,16,14,1000,0},{7,8,9,11,1000,0},{6,3,5,4,1000,0}};

