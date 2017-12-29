/*
 * SerialCommand.h
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 * 
 *  Part of SCMD for Arduino
 */
#include "Arduino.h"
#include <EEPROM.h>
#include "scsmd.h"

#ifndef SerialCommand_h
#define SerialCommand_h

#define INTERFACE Serial
#define MAX_COMMAND_LENGTH 11

struct SerialCommand{
  static char commandString[MAX_COMMAND_LENGTH+1];
  static void parse(char *);
  static void process();
  static void writeCV(char *);
  static void readCV(char *);
  static void writeCVBit(char *);
};
  
#endif

