/*
  Polytone.h - Library for polyphonic tone() generation
  Created by Connor Nishijima, July 16th 2016.
  Released under the GPLv3 license.
*/

#ifndef polytone_h
#define polytone_h

#include "Arduino.h"
#include "digitalWriteFast.h"

class Polytone
{
  public:
    Polytone();
	void begin();
	void end();
	void setTuning(float tune);
	void setPins(byte pin1=0, byte pin2=0, byte pin3=0, byte pin4=0);
	void tone(unsigned int frequency, unsigned long duration=0, byte voice=0);
	void note(byte n, unsigned long duration=0, byte voice=0);
	void stopVoice(byte voice);
	void stopFrequency(unsigned int frequency);
	void stopPin(byte pin);
	void stopAll();
};

#endif
