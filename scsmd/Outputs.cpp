/*
 * Accessory.cpp
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 * 
 * Part of SCMD for Arduino
 */

#include "Outputs.h"

bool Outputs::turnout_switch(Outputs t, bool pos) {
  if(t.polarityInverse)
    pos = !pos;
  if(pos == CLOSE) {
    digitalWrite(t.red_pin, HIGH);
    digitalWrite(t.black_pin, LOW);
    digitalWrite(t.enable_pin, HIGH);
  }
  else if(pos == THROW) {
    digitalWrite(t.red_pin, LOW);
    digitalWrite(t.black_pin, HIGH);
    digitalWrite(t.enable_pin, HIGH);
  }
  return pos;
}

bool Outputs::control_update(Outputs t){
  if(millis() - t.controlChange >= INTERVAL_MILLIS) {
    if(t.controlState != control_read(t)){
      return true;
    }
  }
  return false;
}

bool Outputs::control_read(Outputs t){
  bool pos = (analogRead(t.control_pin) > 254) ? THROW : CLOSE;
  return pos;
}

