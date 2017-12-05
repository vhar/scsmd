/*
 * Single Coil Switch Machine driver
 * Software version 1.0.2b
 * Copyright (c) 2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 * 
 * Change log:
 * 2017-11-24 remapping enable pin to PWM for future using
 *            change control pin processing
 *            extended turnout structure
 * 2017-11-26 added procedure for resetting settings
 *            changed the procedure for switching between operating modes
 * 2017-12-05 fixed bug when switching between operating modes
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "scsmd.h"
#include "scsmd_config.h"
#include <Bounce2.h>
#include <EEPROM.h>
#include <DCC_Decoder.h>
Bounce pushbutton = Bounce();

void BasicAccDecoderPacket_Handler(int address, boolean activate, byte data){
  now = millis();
  if (learningMode > 0) {
    EEPROM.update(EEPROM_DECODER_ADDRESS, address);
    decoderAddress = address;
    learningMode = ledFlag = 0;
    digitalWrite(LEDCONTROL, LOW);      
  } else {
    if (address == decoderAddress) {
      int pos = (data & 0x01) ? THROW : CLOSE;
      int t = (data & 0x06) / 2;
      if(!turnouts[t].motorRun){
        int currentPosition = EEPROM.read(EPPROM_POSITION);
        bitWrite(currentPosition, t, pos);
        EEPROM.update(EPPROM_POSITION, currentPosition);
        turnout_switch(turnouts[t], pos);
        turnouts[t].motorRun = now;
      }
    }
  }
}

void turnout_switch(Turnout t, int pos){
  if(t.polarityInverse)
    pos != pos;
  if(pos == CLOSE){
    digitalWrite(t.red_pin, HIGH);
    digitalWrite(t.black_pin, LOW);
    digitalWrite(t.enable_pin, HIGH);
  }
  else if(pos == THROW){
    digitalWrite(t.red_pin, LOW);
    digitalWrite(t.black_pin, HIGH);
    digitalWrite(t.enable_pin, HIGH);
  }
}

bool control_update(Turnout t){
  if(millis() - t.controlChange >= INTERVAL_MILLIS) {
    bool currentState = control_read(t);
    if(t.controlState != currentState) {
      return true;
    }
  }
  return false;
}

bool control_read(Turnout t){
  bool currentState = analogRead(t.control_pin) > 254 ? THROW : CLOSE;
  return currentState;
}

void decoder_reset(){
  EEPROM.update(EPPROM_POSITION, 0);
  EEPROM.update(EEPROM_DECODER_CONTROL_MODE, DECODER_CONTROL_MODE);
  EEPROM.update(EEPROM_DECODER_OUTPUT_INVERSE, DECODER_OUTPUT_INVERSE);
  EEPROM.update(EEPROM_DECODER_ADDRESS, DECODER_ADDRESS);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Single Coil Switch Machine Driver version "+String(CURRENTVERSION));
  pinMode(PUSHBUTTON, INPUT_PULLUP);
  pinMode(LEDCONTROL, OUTPUT);
  pushbutton.attach(PUSHBUTTON);
  pushbutton.interval(5);
  bool currentState = CLOSE;
  if (digitalRead(PUSHBUTTON) == LOW){
    decoder_reset();
    for (int i=0;i<3;i++) {
      digitalWrite(LEDCONTROL, HIGH);
      delay(CHANGE_MODE_BLINK_TIME);
      digitalWrite(LEDCONTROL, LOW);
      delay(CHANGE_MODE_BLINK_TIME);
    }
  }
  decoderMode = EEPROM.read(EEPROM_DECODER_CONTROL_MODE);
  decoderInverse = EEPROM.read(DECODER_OUTPUT_INVERSE);
  decoderAddress = EEPROM.read(EEPROM_DECODER_ADDRESS);
  DCC.SetBasicAccessoryDecoderPacketHandler(BasicAccDecoderPacket_Handler, true); 
  DCC.SetupDecoder( 0x00, 0x00, kDCC_INTERRUPT );
  Serial.println("Decoder Address "+String(decoderAddress));
  Serial.println("Current control mode:");
  for(int i=0; i<4; i++){
    pinMode(turnouts[i].control_pin, INPUT);
    analogWrite(turnouts[i].control_pin, 255);
    pinMode(turnouts[i].enable_pin, OUTPUT);
    digitalWrite(turnouts[i].enable_pin, LOW);
    pinMode(turnouts[i].red_pin, OUTPUT);
    pinMode(turnouts[i].black_pin, OUTPUT);
    turnouts[i].pulse_time = DECODER_PULSE_TIME * 100;
    turnouts[i].controlMode = bitRead(decoderMode, i);
    Serial.print("Turnout "+String(1+i)+": ");
    if(turnouts[i].controlMode) {
      Serial.print("DCC");
      currentState = bitRead(EEPROM.read(EPPROM_POSITION), i);
    }
    else {
      Serial.print("MANUAL");
      currentState = control_read(turnouts[i]);
      turnouts[i].controlState = currentState;
    }
    turnouts[i].polarityInverse = bitRead(decoderInverse, i);
    turnouts[i].polarityInverse ? Serial.println(" NORMAL") : Serial.println(" INVERSE");
    turnout_switch(turnouts[i], currentState);
    turnouts[i].motorRun = 1;
  }
}

void loop() {
  now = millis();
  if(learningMode) {
    DCC.loop();
  }
  else {
    for(int i=0; i<4; i++){
      if(turnouts[i].motorRun > 0 && turnouts[i].motorRun + turnouts[i].pulse_time < now){
        digitalWrite(turnouts[i].enable_pin, LOW);
        turnouts[i].motorRun = 0;
      }
      if(turnouts[i].controlMode){
        DCC.loop();
      }
      else if(control_update(turnouts[i])){
        bool currentState = turnouts[i].controlState = control_read(turnouts[i]);
        turnouts[i].controlChange = now;
        turnout_switch(turnouts[i], currentState);
        turnouts[i].motorRun = now;
      }
    }
  }
  if(pushbutton.update()){
    if(pushbutton.read() == HIGH){
      pushbuttonHoldTime = 0;
    }
    else {
      pushbuttonHoldTime = now;
    }
  }
  if(pushbuttonHoldTime > 0 && now - pushbuttonHoldTime > PUSHBUTTON_HOLD_TIME) {
    if(learningMode){
      learningMode = ledFlag = 0;
      digitalWrite(LEDCONTROL, LOW);
      EEPROM.update(EEPROM_DECODER_CONTROL_MODE, EEPROM.read(EEPROM_DECODER_CONTROL_MODE) ^ 255);
      decoderMode = EEPROM.read(EEPROM_DECODER_CONTROL_MODE);
      for(int i=0; i<4; i++){
        turnouts[i].controlMode = bitRead(decoderMode, i);
      }
    }
    else {
      learningMode = ledFlag = now;
      flashes_number = 1;
    }
    pushbuttonHoldTime = 0;
  }
  int current_blink_time = LEARNING_BLINK_TIME;      
  if(flashes_count == flashes_number && !digitalRead(LEDCONTROL)){
    current_blink_time = 4 * LEARNING_BLINK_TIME;
  }
  else if(flashes_count == flashes_number)
    flashes_count = 0;
  if(ledFlag && now - ledFlag > current_blink_time){
    flashes_count += digitalRead(LEDCONTROL);
    digitalWrite(LEDCONTROL, !digitalRead(LEDCONTROL));
    ledFlag = now;
  }
  if(learningMode && now - learningMode > LEARNING_MODE_WAIT){
    learningMode = ledFlag = 0;
    digitalWrite(LEDCONTROL, LOW); 
  }
}

