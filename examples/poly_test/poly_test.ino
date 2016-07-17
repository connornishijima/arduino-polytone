#include "Polytone.h"
Polytone poly;

void setup() {
  poly.begin(); // Set up polling timer interrupt
  poly.setPins(4,5,6,7); // Connect pins 4-7 to positive terminal of speaker/piezo

  delay(500);

  // C CHORD sweep
  poly.note(52);
  delay(50);
  poly.note(56);
  delay(50);
  poly.note(59);
  delay(50);
  poly.note(64);
  delay(500);

  // G CHORD
  poly.note(47); poly.note(51); poly.note(51); poly.note(59);
  delay(500);

  // Am CHORD
  poly.note(49); poly.note(53); poly.note(56); poly.note(61);
  delay(500);

  // F CHORD
  poly.note(45,500); poly.note(49,700); poly.note(52,900); poly.note(57,1100);
  delay(1500);

  // 440Hz for 500ms on voice 3;
  poly.tone(440,500,3);

  // 262Hz for 500ms on voice 1;
  poly.tone(262,500,1);
  
  delay(1000);

  // Sweep piano keys both directions
  unsigned int key = 0;
  while(key < 88){
    poly.note(key,500,1);
    poly.note(88-key,500,2);
    delay(50);
    key++;
  }
  
  poly.end(); // Clear polling timer interrupt
}

void loop() {
  // put your main code here, to run repeatedly:

}
