/*
 * scsm.h
 * Single Coil Machine Driver configuration file
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 */

#define PUSHBUTTON 19 // Push button pin
#define LEDCONTROL 13 // Pin for connection the controlling LED. By default used onboard Arduino LED.
// If no packet with an address has been received during this time, the decoder returns to the operating mode
#define DECODER_ADDRESS 1 // Default decoder DCC address
/*
 * Turnouts structures array. Mode details see on scsm.h file
 * {control_pin, enable_pin, red_pin, black_pin, pulse_time, motorRun}
 */
Turnout turnouts[]={{14,15,16,17,1000,0},{12,10,13,11,1000,0},{6,7,8,9,1000,0},{5,18,4,3,1000,0}};

