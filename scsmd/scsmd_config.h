/*
 * scsm.h
 * Single Coil Machine Driver configuration file
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 */
#define PUSHBUTTON 8 // Push button pin
#define LEDCONTROL 13 // Pin for connection the controlling LED. By default used onboard Arduino LED.
//decoder default values
#define DECODER_CONTROL_MODE 15 // The lower 4 bytes define the output control mode. 0 = MANUAL, 1 = DCC.
#define DECODER_OUTPUT_INVERSE 15 // The lower 4 bytes define the inverse output polarity. 0 = INVERSE, 1 = NORMAL.
#define DECODER_ADDRESS 3 // Default decoder DCC address
#define DECODER_PULSE_TIME 10 // Default output pulse time (x* 100ms)

