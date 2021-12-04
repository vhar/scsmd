/*
 * scsmd.h
 * Single Coil Machine Driver Interface
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 * 
 * Part of SCMD for Arduino
 */

#ifndef scsmd_h
#define scsmd_h

#define CURRENTVERSION 202
#define kDCC_INTERRUPT 0 // Pin 2 used as DCC Interrupt
#define PUSHBUTTON_HOLD_TIME 1500 // Push button hold time (in milliseconds) before switching mode.
#define LEARNING_MODE_WAIT 15000 // The DCC address waiting time (in milliseconds) in the Learning Mode.
// If no packet with an address has been received during this time, the decoder returns to the operating mode
#define LEARNING_BLINK_TIME 200 // LED blink time (in milliseconds) on Learning Mode
#define CHANGE_MODE_BLINK_TIME 1000 // Flashing time of the LED (in milliseconds) when the operating mode changes

struct scsmd{
  static void decoder_read();
  static void decoder_reset();
  static void decoder_print();
};

#endif
