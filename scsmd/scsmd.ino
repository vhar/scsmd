/*
   Single Coil Switch Machine driver
   Software version 2.0.1b
   Copyright (c) 2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/

   Change log:
   2017-11-24 remapping enable pin to PWM for future using
              change control pin processing
              extended turnout structure
   2017-11-26 added procedure for resetting settings
              changed the procedure for switching between operating modes
   2017-12-05 fixed bug when switching between operating modes
   2017-12-29 added Configuration Variables
              СV setting is possible via serial port
              the operating mode can be set for each output separately
              the DCC address is set for each output separately
 
   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <Bounce2.h>
#include <EEPROM.h>
#include <DCC_Decoder.h>
#include "scsmd.h"
#include "scsmd_config.h"
#include "Outputs.h"
#include "SerialCommand.h"

Bounce pushbutton = Bounce();
Outputs turnouts[] = {{21, 6, 16, 17, 0, 0, 0, 0, 1, 1, 1}, {20, 11, 15, 14, 0, 0, 0, 0, 1, 1, 2}, {19, 9, 10, 12, 0, 0, 0, 0, 1, 1, 3}, {18, 3, 5, 4, 0, 0, 0, 0, 1, 1, 4}};

unsigned long learningMode = 0; // Timer flag for DCC address learning mode.
unsigned long pushbuttonHoldTime = 0; // Timer flag for pushbutton.
unsigned long ledFlag = 0; // Timer flag for LED control.
unsigned long now; // variable for operative storing current millis
int flashes_number = 0; // number of flashes in LEARNING mode. For future use.
int flashes_count = 0; // count number of flashes in LEARNING mode. For future use.

void BasicAccDecoderPacket_Handler(int address, boolean activate, byte data) {
  now = millis();
  // Convert NMRA packet address format to human address
  address -= 1;
  address *= 4;
  address += 1;
  address += (data & 0x06) >> 1;
  if (learningMode > 0) {
    int i = flashes_number - 1;
    int lsb = (address % 256);
    EEPROM.update(EEPROM_DECODER_ADDRESS_LSB + i, lsb);
    int msb = (address / 256);
    EEPROM.update(EEPROM_DECODER_ADDRESS_MSB + i, msb);
    turnouts[i].address = address;
    learningMode = ledFlag = 0;
    digitalWrite(LEDCONTROL, LOW);
  } else {
    for (int i = 0; i < 4; i++) {
      if (address == turnouts[i].address && !turnouts[i].controlMode) {
        int pos = (data & 0x01) ? THROW : CLOSE;
        if (!turnouts[i].motorRun) {
          int currentPosition = EEPROM.read(EPPROM_POSITION);
          bitWrite(currentPosition, i, pos);
          EEPROM.update(EPPROM_POSITION, currentPosition);
          Outputs::turnout_switch(turnouts[i], pos);
          turnouts[i].motorRun = now;
        }
      }
    }
  }
}

void scsmd::decoder_read() {
  bool currentState = CLOSE;
  int decoderMode = EEPROM.read(EEPROM_DECODER_CONFIGURATION);
  for (int i = 0; i < 4; i++) {
    turnouts[i].address = EEPROM.read(EEPROM_DECODER_ADDRESS_LSB + i) + (EEPROM.read(EEPROM_DECODER_ADDRESS_MSB + i) * 256);
    turnouts[i].pulse_time = EEPROM.read(EEPROM_DECODER_PULSE_TIME + i) * 100;
    turnouts[i].controlMode = bitRead(decoderMode, i);
    if (turnouts[i].controlMode) {
      Outputs::turnout_switch(turnouts[i], bitRead(EEPROM.read(EPPROM_POSITION), i));
    }
    else {
      turnouts[i].controlState = Outputs::control_read(turnouts[i]);
      Outputs::turnout_switch(turnouts[i], turnouts[i].controlState);
    }
    turnouts[i].polarityInverse = bitRead(decoderMode, i + 4);
    turnouts[i].motorRun = 1;
  }
}

void scsmd::decoder_print() {
  INTERFACE.println("Single Coil Switch Machine Driver version " + String(CURRENTVERSION));
  INTERFACE.println("Current control mode:");
  for (int i = 0; i < 4; i++) {
    INTERFACE.print("Turnout " + String(1 + i) + ": ");
    pinMode(turnouts[i].control_pin, INPUT);
    analogWrite(turnouts[i].control_pin, 255);
    pinMode(turnouts[i].enable_pin, OUTPUT);
    digitalWrite(turnouts[i].enable_pin, LOW);
    pinMode(turnouts[i].red_pin, OUTPUT);
    pinMode(turnouts[i].black_pin, OUTPUT);
    turnouts[i].controlMode ? INTERFACE.print("MANUAL") : INTERFACE.print("DCC " + String(turnouts[i].address));
    turnouts[i].polarityInverse ? INTERFACE.println(" INVERSE " + String(turnouts[i].pulse_time) + "ms") : INTERFACE.println(" NORMAL " + String(turnouts[i].pulse_time) + "ms");
  } 
}

void scsmd::decoder_reset() {
  EEPROM.update(EPPROM_POSITION, 0);
  EEPROM.update(EEPROM_DECODER_CONFIGURATION, DECODER_CONFIGURATION);
  for (int i = 0; i < 4; i++) {
    EEPROM.update(EEPROM_DECODER_PULSE_TIME + i, DECODER_PULSE_TIME);
    EEPROM.update(EEPROM_DECODER_PWM + i, DECODER_PWM);
    int lsb = ((DECODER_ADDRESS + i) % 256);
    EEPROM.update(EEPROM_DECODER_ADDRESS_LSB + i, lsb);
    int msb = (DECODER_ADDRESS + i) / 256;
    EEPROM.update(EEPROM_DECODER_ADDRESS_MSB + i, msb);
  }
}


void setup() {
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;

  INTERFACE.begin(115200);
  pinMode(PUSHBUTTON, INPUT_PULLUP);
  pinMode(LEDCONTROL, OUTPUT);
  pushbutton.attach(PUSHBUTTON);
  pushbutton.interval(5);
  if (digitalRead(PUSHBUTTON) == LOW) {
    scsmd::decoder_reset();
    for (int i = 0; i < 3; i++) {
      digitalWrite(LEDCONTROL, HIGH);
      delay(CHANGE_MODE_BLINK_TIME);
      digitalWrite(LEDCONTROL, LOW);
      delay(CHANGE_MODE_BLINK_TIME);
    }
  }
  scsmd::decoder_read();
  scsmd::decoder_print();
  DCC.SetBasicAccessoryDecoderPacketHandler(BasicAccDecoderPacket_Handler, true);
  DCC.SetupDecoder( 0x00, 0x00, kDCC_INTERRUPT );
}

void loop() {
  now = millis();
  SerialCommand::process();
  if (learningMode) {
    DCC.loop();
  }
  else {
    for (int i = 0; i < 4; i++) {
      if (turnouts[i].motorRun > 0 && turnouts[i].motorRun + turnouts[i].pulse_time < now) {
        digitalWrite(turnouts[i].enable_pin, LOW);
        turnouts[i].motorRun = 0;
      }
      DCC.loop();
      if (turnouts[i].controlMode && Outputs::control_update(turnouts[i])) {
        turnouts[i].controlState = Outputs::control_read(turnouts[i]);
        turnouts[i].controlChange = now;
        bool pos = Outputs::turnout_switch(turnouts[i], turnouts[i].controlState);
        turnouts[i].motorRun = now;
      }
    }
  }
  if(pushbutton.update()){
    if(pushbutton.read() == HIGH){
      if(learningMode && pushbuttonHoldTime){
        learningMode = ledFlag = now;
        (flashes_number > 3) ? flashes_number = 1 : flashes_number++;
        digitalWrite(LEDCONTROL, LOW);
        flashes_count = flashes_number;
      }
      pushbuttonHoldTime = 0;
    }
    else {
      pushbuttonHoldTime = now;
    }
  }
  if(pushbuttonHoldTime > 0 && now - pushbuttonHoldTime > PUSHBUTTON_HOLD_TIME) {
    if(learningMode){
      int bNum = flashes_number-1;
      learningMode = ledFlag = flashes_number = flashes_count = 0;
      digitalWrite(LEDCONTROL, LOW);
      int decoderMode = EEPROM.read(EEPROM_DECODER_CONFIGURATION);
      bool bValue = bitRead(decoderMode, bNum);
      bValue ? bitClear(decoderMode, bNum) : bitSet(decoderMode, bNum);
      EEPROM.update(EEPROM_DECODER_CONFIGURATION, decoderMode);
      turnouts[bNum].controlMode = !bValue;
    }
    else {
      learningMode = ledFlag = now;
      flashes_number = 1;
    }
    pushbuttonHoldTime = 0;
  }
  int current_blink_time = LEARNING_BLINK_TIME;
  if (flashes_count == flashes_number && !digitalRead(LEDCONTROL)) {
    current_blink_time = 4 * LEARNING_BLINK_TIME;
  }
  else if (flashes_count == flashes_number)
    flashes_count = 0;
  if (ledFlag && now - ledFlag > current_blink_time) {
    flashes_count += digitalRead(LEDCONTROL);
    digitalWrite(LEDCONTROL, !digitalRead(LEDCONTROL));
    ledFlag = now;
  }
  if (learningMode && now - learningMode > LEARNING_MODE_WAIT) {
    learningMode = ledFlag = 0;
    digitalWrite(LEDCONTROL, LOW);
  }
}

