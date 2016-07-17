#Arduino Polytone Library

Ever needed a project to play more than ONE tone at once? Using ultra-fast timer interrupt polling, the Polytone library allows an Arduino to generate up to four notes at once!

----------
# Contents
- [Installation](#installation)
- [Usage](#usage)
- [Functions](#functions)
- [Limitations](#limitations)
- [Contributing](#contributing)
- [License and credits](#license-and-credits)

----------
# Installation

**With Arduino Library Manager:**

1. Open *Sketch > Include Library > Manage Libraries* in the Arduino IDE.
2. Search for "Polytone", (look for "Connor Nishijima") and select the latest version.
3. Click the Install button and Arduino will prepare the library and examples for you!

**Manual Install:**

1. Click "Clone or Download" above to get an "arduino-polytone-master.zip" file.
2. Extract it's contents to the libraries folder in your sketchbook.
3. Rename the folder from "arduino-polytone-master" to "Polytone".

----------
# Usage

Using the `poly.tone()` function looks very similar to the Arduino `tone()`, but the function arguments are very different:

**Arduino:**
 - **tone**(unsigned int **pin**, unsigned int **frequency** [, unsigned long **duration**]);

**Polytone:**
 - poly.**tone**(unsigned int **frequency** [, unsigned long **duration** [, byte **voice**]]);

----------
Here is what you need to get started with the bare minimum:

    #include "Polytone.h" // Include the Polytone library

    Polytone poly;

    void setup() {
      poly.begin(); // Set up polling timer interrupt
      poly.setPins(4,5); // Connect pins 4-5 to positive terminal of speaker/piezo

      poly.tone(262); // generates 262Hz on voice 0 (pin 4)
      poly.tone(440); // generates 440Hz on voice 1 (pin 5)
      delay(1000);

      poly.end();
    }
    void loop() {
    }

----------
# Functions

**Polytone poly**;

This initializes the Polytone library after import. "poly" can be any word you want, as long as it's reflected in the rest of your code.

**poly.begin**();

This sets up a Timer Compare Interrupt on Timer1 for software polling. (You won't hear anything until a `poly.tone() or poly.note()` is called.)

**poly.tone**(unsigned int **frequency** [, unsigned long **duration** [, byte **voice**]]);

This is used to start generating a frequency on an open voice pin, or the oldest if we ran out of voices. Optional arguments are duration (infinite if not specified) and the specific voice to generate the tone on.

**poly.note**(byte **n** [, unsigned long **duration** [, byte **voice**]]);

This is an alternative to poly.tone() that accepts notes numbers instead: n = piano key number 1-88.

**poly.setTuning**(float **tune**);

Used to pitch output up or down, in case it's sharp or flat.

**poly.stopVoice**(byte **voice**);

Stops tone production of a voice (1 -> 4)

**poly.stopFrequency**(byte **frequency**);

Stops tone production on all voices with a matching frequency

**poly.stopPin**(byte **pin**);

Stops tone production on an Arduino pin.

----------
# Supported Pins

By default, the library uses DEFAULT_PIN for the speaker, *(changes from board to board due to Timer0 channels)* but if you need this pin for digitalWrite's, you can call *vol.alternatePin(**true**)* to use ALTERNATE_PIN instead.

| Board                           | DEFAULT_PIN | ALTERNATE_PIN | Tested |
|---------------------------------|-------------|---------------|--------|
| (**Uno**) ATmega168/328(pb)     | 5           | 6             | YES    |
| (**Mega**) ATmega1280/2560      | 4           | 13            | YES    |
| (**Leo/Micro**) ATmega16u2/32u4 | 9           | 10            | YES*   |

*I recently killed my only ATmega32u4 board while stripping it for low-power usage and don't have one to test current releases of the library. If anyone who has a working one wants to report compatibility back to me, please do so as I've only tested the initial release!

----------
# Limitations
Unfortunately, cheating the Arduino's normal functions in this way means we'll lose some of them. This is also still a proof-of-concept library at this point, so it may break more functionality than I'm aware of. Sorry!

~~**16MHz Only:**~~

~~I haven't programmed in options for 8MHz boards yet, though if you want to use one, just replace all occurrences of "16000000" in the library files with "8000000" and it may work.~~

Automatic detection of CPU speed was added in version 1.0.2!

**ATmega* Only:**

I don't know if I'll have this working on ATTiny*5 boards any time soon, though it's theoretically possible on any AVR with >= 2 timers. For now, it's only confirmed working on Arduino Uno (ATMega168/328) and Mega. (ATMega1280/2560)

**Volume is limited to pins ~~5 & 6:~~**

This is because only pins ~~5 & 6~~ are driven by Timer0, *which can do PWM at a frequency higher than your hearing range!* This is the main trick behind the volume function. It also means that while you're using Volume, normal `analogWrite()` use probably won't work on these two pins.

*Now that the Mega168, 328, 1280, 2560, 16u2/32u4 and more are now supported, the supported pins differs from board to board. See [Supported Pins](#supported-pins) section.*

**Volume alters Timer0 for 62.5KHz PWM:**

Speaking of Timer0 - it's normally used for the `delay()`, `delayMicroseconds()`, `millis()` and `micros()` functions. Normally with Timer0 set with a divisor of 64, `delay(1000)` would wait for 1 second - but because Volume sets Timer0 with a divisor of 1, `delay(1000)` will now only wait for 15.625ms! But don't worry. Volume provides alternative `vol.delay(time)` and `vol.delayMicroseconds(time)` functions with the math fixed for you. This new divisor is necessary to drive PWM at 62.5KHz, faster than you can hear.

~~**Volume does not yet offer fixed millis() or micros() functions:**~~

~~I haven't gotten around to toying with these yet. If you need to use `millis()` or `micros()` BETWEEN playing sounds, just use a `vol.end()` to reset Timer0 to it's default function, and `vol.begin()` to use it for Volume again after you're done.~~

Version 1.1.1 added proper millis() and micros() support! See [Functions](#functions).

----------
# Contributing
As I've only written one library before for my own use, I'm still new to this! Any advice or pull requests are welcome. :)

----------
# License and Credits
**Developed by Connor Nishijima (2016)**

**Special Thanks to Andrew Neal** (For putting up with the incessant and inconsistent artificial "cricket-duino" hidden in his vent that I developed this library for.)

**Released under the [GPLv3 license](http://www.gnu.org/licenses/gpl-3.0.en.html).**
