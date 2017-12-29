/*
 * SerialCommand.cpp
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 * 
 *  Part of SCMD for Arduino
 */

#include "SerialCommand.h"
char SerialCommand::commandString[MAX_COMMAND_LENGTH+1];

void SerialCommand::process(){
  char c;
  while(INTERFACE.available()>0){
    c=INTERFACE.read();
    if(c=='<')
      sprintf(commandString,"");
    else if(c=='>')
      parse(commandString);                    
    else if(strlen(commandString)<MAX_COMMAND_LENGTH)
      sprintf(commandString,"%s%c",commandString,c);
  }
}

void SerialCommand::parse(char *com){
  switch(com[0]){
/***** WRITE CONFIGURATION VARIABLE ****/
    case 'W':      // <W CV VALUE>
/*
 *    writes, without any verification, a Configuration Variable to the decoder
 *    
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    VALUE: the value to be written to the Configuration Variable memory location (0-255)
 *    
 *    returns: <r CV VALUE>
 *    where VALUE is a number from 0-255 as read from the requested CV
*/    
      writeCV(com+1);
    break;
/***** READ CONFIGURATION VARIABLE ****/
    case 'R':     // <R CV>
/*    
 *    reads a Configuration Variable from the decoder
 *    
 *    CV: the number of the Configuration Variable memory location in the decoder to read from (1-1024)
 *    
 *    returns: <r CV VALUE>
 *    where VALUE is a number from 0-255 as read from the requested CV
*/    
      readCV(com+1);
    break;
/***** WRITE CONFIGURATION VARIABLE BIT ****/ 
    case 'B':      // <B CV BIT VALUE>
/*
 *    writes, without any verification, a single bit within a Configuration Variable to the decoder
 *    
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    BIT: the bit number of the Configurarion Variable regsiter to write (0-7)
 *    VALUE: the value of the bit to be written (0-1)
 *    
 *    returns: <r CV BIT VALUE>
*/        
      writeCVBit(com+1);
    break;      
  }
}

void SerialCommand::writeCV(char *s) {
  int cv;
  int bValue;
  if(sscanf(s,"%d %d",&cv,&bValue)!=2)
    return;
  if(bValue > 255)
    return;
  cv = cv - 512;
  EEPROM.update(cv, bValue);
  scsmd::decoder_read();
  INTERFACE.print("<r ");
  INTERFACE.print(cv+512);
  INTERFACE.print(" ");
  INTERFACE.print(bValue);
  INTERFACE.println(">");
}

void SerialCommand::writeCVBit(char *s){
  int bNum,bValue;
  int cv;
  if(sscanf(s,"%d %d %d",&cv,&bNum,&bValue)!=3)
    return;
  if(bNum > 7)
    return;
  if(bValue > 1)
    return;
  cv = cv - 512;
  int base = EEPROM.read(cv);
  bValue ? bitSet(base, bNum) : bitClear(base, bNum);
  EEPROM.update(cv, base);
  scsmd::decoder_read();
  INTERFACE.print("<r ");
  INTERFACE.print(cv+512);
  INTERFACE.print(" ");
  INTERFACE.print(bNum);
  INTERFACE.print(" ");
  INTERFACE.print(bValue);
  INTERFACE.println(">");
}

void SerialCommand::readCV(char *s){
  int cv;
  int bValue;
  if(sscanf(s,"%d",&cv)!=1)
    return;
  cv = cv - 512;
  bValue = EEPROM.read(cv);
  INTERFACE.print("<r ");
  INTERFACE.print(cv + 512);
  INTERFACE.print(" ");
  INTERFACE.print(bValue);
  INTERFACE.println(">");
}

