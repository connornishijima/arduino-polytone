/*
  Polytone.cpp - Library for polyphonic tone() generation
  Created by Connor Nishijima, July 16th 2016.
  Released under the GPLv3 license.
*/

#include "Polytone.h"

float tuning = 1.0;
byte voicePins[4];
byte voiceCount = 0;
unsigned long periods[4];
unsigned long lastFlip[4];
unsigned long expirations[4];
unsigned long starts[4];
byte states[4];
unsigned int iteration = 0;
unsigned long t = 0;
unsigned long cpuCycles = 0;

const unsigned int notes[89] = {
  0, 27, 29, 30,
  33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,
  65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 116, 123,
  131, 138, 147, 155, 165, 175, 185, 196, 208, 220, 233, 247,
  262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
  523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
  1046, 1109, 1175, 1244, 1318, 1397, 1480, 1568, 1661, 1760, 1865, 1975,
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
  4186,
};

void Polytone::begin() {
  // TIMER 1 for interrupt frequency 40000 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 40000 Hz increments
  OCR1A = 399; // = 16000000 / (1 * 40000) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts
}

void Polytone::end() {
	TIMSK1 = 0;
	stopAll();
}

Polytone::Polytone()
{
}

void Polytone::setPins(byte pin1, byte pin2, byte pin3, byte pin4) {
  if (pin1 != 0) {
    voicePins[0] = pin1;
  }
  if (pin2 != 0) {
    voicePins[1] = pin2;
  }
  if (pin3 != 0) {
    voicePins[2] = pin3;
  }
  if (pin4 != 0) {
    voicePins[3] = pin4;
  }
}

void Polytone::setTuning(float tune){
  tuning = tune;
}

void Polytone::tone(unsigned int frequency, unsigned long duration, byte voice) {
  frequency *= tuning;
  if(frequency < 0){
    frequency = 0;
  }
  unsigned long start = millis();
  byte assignment = 255;
  if (voice == 0) {
    for (byte i = 0; i < 4; i++) {
      if (expirations[i] == 0) {
        assignment = i;
        break;
      }
    }
    if (assignment == 255) {
      unsigned long mn = 0 - 1;
      for (byte i = 0; i < 4; i++) {
        if (starts[i] < mn) {
          mn = starts[i];
          assignment = i;
        }
      }
    }
  }
  else {
    assignment = voice - 1;
  }
  periods[assignment] = F_CPU / (frequency * 2);
  if (duration > 0) {
    expirations[assignment] = start + duration;
  }
  else {
    expirations[assignment] = 4294967295;
  }
  starts[assignment] = start;
  pinModeFast(voicePins[assignment], INPUT_PULLUP);
}

void Polytone::note(byte n, unsigned long duration, byte voice){
	Polytone::tone(notes[n],duration,voice);
}

void Polytone::stopVoice(byte voice) {
  expirations[voice-1] = 1;
  digitalWriteFast2(voicePins[voice-1], LOW);
  pinModeFast(voicePins[voice-1], INPUT_PULLUP);
}

void Polytone::stopFrequency(unsigned int frequency) {
  unsigned int per = F_CPU / (frequency * 2);
  for (byte i = 0; i < 4; i++) {
    if (per == periods[i]) {
      expirations[i] = 1;
	  digitalWriteFast2(voicePins[i], LOW);
      pinModeFast(voicePins[i], INPUT_PULLUP);
    }
  }
}

void Polytone::stopPin(byte pin) {
  for (byte i = 0; i < 4; i++) {
    if (voicePins[i] == pin) {
      expirations[i] = 1;
	  digitalWriteFast2(voicePins[i], LOW);
      pinModeFast(voicePins[i], INPUT_PULLUP);
    }
  }
}

void Polytone::stopAll() {
  
  pinModeFast(voicePins[0],OUTPUT);
  pinModeFast(voicePins[1],OUTPUT);
  pinModeFast(voicePins[2],OUTPUT);
  pinModeFast(voicePins[3],OUTPUT);
  
  digitalWriteFast2(voicePins[0], LOW);
  digitalWriteFast2(voicePins[1], LOW);
  digitalWriteFast2(voicePins[2], LOW);
  digitalWriteFast2(voicePins[3], LOW);
  
  expirations[0] = 0;
  expirations[1] = 0;
  expirations[2] = 0;
  expirations[3] = 0;
}

ISR(TIMER1_COMPA_vect) {
  //interrupt commands for TIMER 1 here
  cpuCycles += 400;
  iteration++;

  if (iteration >= 1000) {
    unsigned long now = millis();
    iteration = 0;
    for (byte i = 0; i < 4; i++) {
      if (expirations[i] != 0 && now >= expirations[i] && expirations[i] != 4294967295) {
        expirations[i] = 0;
        digitalWriteFast2(voicePins[i], LOW);
      }
    }
  }

  if (expirations[0] != 0) {
    if (cpuCycles - lastFlip[0] >= periods[0]) {
      lastFlip[0] = cpuCycles;
      states[0] = !states[0];
      if (states[0] == 1) {
        pinModeFast(voicePins[0], OUTPUT);
        digitalWriteFast2(voicePins[0], HIGH);
      }
      else if (states[0] == 0) {
        digitalWriteFast2(voicePins[0], LOW);
        pinModeFast(voicePins[0], INPUT_PULLUP);
      }
    }
  }

  if (expirations[1] != 0) {
    if (cpuCycles - lastFlip[1] >= periods[1]) {
      lastFlip[1] = cpuCycles;
      states[1] = !states[1];
      if (states[1] == 1) {
        pinModeFast(voicePins[1], OUTPUT);
        digitalWriteFast2(voicePins[1], HIGH);
      }
      else if (states[1] == 0) {
        digitalWriteFast2(voicePins[1], LOW);
        pinModeFast(voicePins[1], INPUT_PULLUP);
      }
    }
  }

  if (expirations[2] != 0) {
    if (cpuCycles - lastFlip[2] >= periods[2]) {
      lastFlip[2] = cpuCycles;
      states[2] = !states[2];
      if (states[2] == 1) {
        pinModeFast(voicePins[2], OUTPUT);
        digitalWriteFast2(voicePins[2], HIGH);
      }
      else if (states[2] == 0) {
        digitalWriteFast2(voicePins[2], LOW);
        pinModeFast(voicePins[2], INPUT_PULLUP);
      }
    }
  }

  if (expirations[3] != 0) {
    if (cpuCycles - lastFlip[3] >= periods[3]) {
      lastFlip[3] = cpuCycles;
      states[3] = !states[3];
      if (states[3] == 1) {
        pinModeFast(voicePins[3], OUTPUT);
        digitalWriteFast2(voicePins[3], HIGH);
      }
      else if (states[3] == 0) {
        digitalWriteFast2(voicePins[3], LOW);
        pinModeFast(voicePins[3], INPUT_PULLUP);
      }
    }
  }
}
