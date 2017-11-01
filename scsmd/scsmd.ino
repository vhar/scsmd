/*
 * Single Coil Switch Machine driver
 * Software version 0.02b
 * Copyright (c) 2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 * 
 * Change log:
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
Bounce points[4] = Bounce();
Bounce pushbutton = Bounce();
String CURRENTVERSION = "0.02b";

void setup() {
  Serial.begin(115200);
  Serial.println("Single Coil Switch Machine Driver version "+CURRENTVERSION);
  boolean pos = CLOSE;
  pinMode(PUSHBUTTON, INPUT_PULLUP);
  pinMode(LEDCONTROL, OUTPUT);
  pushbutton.attach(PUSHBUTTON);
  pushbutton.interval(5);
  if(EEPROM.read(EEPROM_MODE) > 1) {
    EEPROM.update(EEPROM_MODE, 1);
  }
  if (digitalRead(PUSHBUTTON) == LOW){
    EEPROM.update(EEPROM_MODE, !EEPROM.read(EEPROM_MODE));
    for (int i=0;i<2+EEPROM.read(EEPROM_MODE);i++) {
      digitalWrite(LEDCONTROL, HIGH);
      delay(CHANGE_MODE_BLINK_TIME);
      digitalWrite(LEDCONTROL, LOW);
      delay(CHANGE_MODE_BLINK_TIME);
    }
  }
  decoderMode = EEPROM.read(EEPROM_MODE);
  if(EEPROM.read(EEPROM_ADDRESS) == 255) {
    EEPROM.update(EEPROM_ADDRESS, DECODER_ADDRESS);
  }
  Serial.print("Current mode: ");
  if(decoderMode){
    Serial.println("DCC");
    DCC.SetBasicAccessoryDecoderPacketHandler(BasicAccDecoderPacket_Handler, true); 
    DCC.SetupDecoder( 0x00, 0x00, kDCC_INTERRUPT );
    decoderAddress = EEPROM.read(EEPROM_ADDRESS);
    Serial.println("Decoder Address "+String(decoderAddress));
  }
  else {
    Serial.println("MANUAL");
  }
  for(int i=0; i<4; i++){
    pinMode(turnouts[i].control_pin, INPUT_PULLUP);
    points[i].attach(turnouts[i].control_pin);
    points[i].interval(5);
    pinMode(turnouts[i].enable_pin, OUTPUT);
    digitalWrite(turnouts[i].enable_pin, LOW);
    pinMode(turnouts[i].red_pin, OUTPUT);
    pinMode(turnouts[i].black_pin, OUTPUT);
    if(decoderMode) {
      if(EEPROM.read(EPPROM_POSITION + i) > 1) {
        EEPROM.update(EPPROM_POSITION + i, CLOSE);
      }
      pos = EEPROM.read(EPPROM_POSITION + i);
    }
    else {
      pos = digitalRead(turnouts[i].control_pin);
    }
    turnout_switch(turnouts[i], pos);
    turnouts[i].motorRun = 1;    
  }
  digitalWrite(LEDCONTROL, decoderMode);
}

void loop() {
  now = millis();
  for(int i=0; i<4; i++){
    if(turnouts[i].motorRun > 0 && turnouts[i].motorRun + turnouts[i].pulse_time < now){
      digitalWrite(turnouts[i].enable_pin, LOW);
      turnouts[i].motorRun = 0;
    }
    if(decoderMode){
      DCC.loop();
    }
    else if(points[i].update()){
      turnout_switch(turnouts[i], points[i].read());
      turnouts[i].motorRun = now;
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
  if(decoderMode){
    if (pushbuttonHoldTime > 0 && now - pushbuttonHoldTime > PUSHBUTTON_HOLD_TIME) {
      if(learningMode){
        learningMode = ledFlag = 0;
        digitalWrite(LEDCONTROL, decoderMode);
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
      digitalWrite(LEDCONTROL, decoderMode);      
    }
  }
}

void BasicAccDecoderPacket_Handler(int address, boolean activate, byte data) { 
  now = millis();
  if (learningMode > 0) {
    EEPROM.update(EEPROM_ADDRESS, address);
    decoderAddress = address;
    learningMode = ledFlag = 0;
    digitalWrite(LEDCONTROL, decoderMode);      
  } else {
    if (address == decoderAddress) {
      int pos = (data & 0x01) ? THROW : CLOSE;
      int t = (data & 0x06) / 2;
      if(!turnouts[t].motorRun){
        turnout_switch(turnouts[t], pos);
        turnouts[t].motorRun = now;
        EEPROM.update(EPPROM_POSITION + t, pos);
      }
    }
  }
}

