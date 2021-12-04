/*
 * Accessory.h
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 * 
 * Part of SCMD for Arduino
 */

#include "Arduino.h"
#include <EEPROM.h>

#ifndef Accessory_h
#define Accessory_h

#define CLOSE 0 // Turnout closed position signal level
#define THROW 1 // Turnout throwed position signal level
#define INTERVAL_MILLIS 10 // Control Inputs debounce timeout

#define EPPROM_POSITION 0 // EEPROM address for storing turnout position on DCC mode.
#define EEPROM_DECODER_CONFIGURATION 2 // EEPROM address for storing the outup inverse and control mode
        // CV514 lower 4 bites (0-3) - define the output control mode: 0 = DCC, 1 = MANUAL. 
        // The upper 4 bites (4-7) define the inverse output polarity. 0 = NORMAL, 1 = INVERSE.
#define EEPROM_DECODER_PULSE_TIME 3 // starting EEPROM address for storing the output pulse time.
        // CV515-CV518 = x*100ms
#define EEPROM_DECODER_PWM 33 // starting EEPROM address for storing the output PWM value.
        // CV545 - CV548
        // PWM value also determines the decoder output mode. 0 = pulse, 255 = permanent, 1-254 pulse and hold 
        // values from 0 to 255 is part of hold output voltage. 0 = 0, 255 = full.
#define EEPROM_DECODER_ADDRESS_LSB 43 // starting EEPROM address for storing low-order address bits (LSB) of output DCC address.
        //CV555 - CV558. Contains the address value results from the following formula: Output Address modulus 256
#define EEPROM_DECODER_ADDRESS_MSB 51 // starting EEPROM address for storing DCC high-order address bits (MSB) of output DCC address. 
        // CV563 - CV566. Contains the address value results from the quotient of the following formula: Output Address divided by 256
        // Please, don't configure this values. Use LEARNING mode.
#define DECODER_ADDRESS 1 // Default decoder DCC address
        // Output #1 - Accessory address 1
        // Output #2 - Accessory address 2
        // Output #3 - Accessory address 3
        // Output #4 - Accessory address 4        
#define DECODER_PULSE_TIME 10 // Default output pulse time (x* 100ms)
#define DECODER_PWM 0 //Default PWM value is 0, decoder outputs mode = PULSE
#define DECODER_CONFIGURATION 0 //Default output control and inverse mode

/* 
 * Turnout structure
 * control_pin - pull up input used for turnout control in manual mode
 * enable_pin - connection to L293D "Enable" pin
 * red_pin - connection to L293D "Input 2"  pin
 * black_pin - connection to L293D "Input 1"  pin
 * pulse_time - pulse width in x*100ms on L293D output (10 = 1000ms)
 * motorRun - Timer flag for turnout switching process
 * controlState - current signal level on control_pin. 
 * controlChange - Timer flag for store last change signal lavel on control_pin.
 * controlMode - current output control mode. 0 = MANUAL, 1 = DCC.
 * polarityInverse - inverse output polarity. 0 = INVERSE, 1 = NORMAL.
 */
struct Outputs
{
  int control_pin;
  int enable_pin;
  int red_pin;
  int black_pin;
  int pulse_time;
  unsigned long motorRun;
  bool controlState;
  unsigned long controlChange;
  bool controlMode;
  bool polarityInverse;
  int address; 
  static bool turnout_switch(Outputs, bool);
  static bool control_update(Outputs);
  static bool control_read(Outputs);
};
#endif

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
