/*
    Still Alive
     
    Plays a the song Still Alive
     
    
    circuit:
    * 8-ohm speaker on digital pin 8
     
    created 21 Jan 2010
    modified 14 Oct 2010
    by Tom Igoe
    This example code is in the public domain.
     

*/

#include "pitches.h"
#define REST 0
double song[] = {
     
    // First line
    REST, 2.0, NOTE_G3, 0.5, NOTE_FS3, 0.5, NOTE_E3, 0.5, NOTE_E3, 0.5,
    NOTE_FS3, 2.0, REST, 2.0,
     
    REST, 1.5, NOTE_A2, 0.5,
    NOTE_G3, 0.5, NOTE_FS3, 0.5, NOTE_E3, 0.5, NOTE_E3, 1.0,
     
    NOTE_FS3, 1.5,
    NOTE_D3, 1.0, NOTE_E3, 0.5, NOTE_A2, 2.5,
     
    REST, 1.5, NOTE_A2, 0.5,
     
    // Second line
    NOTE_E3, 1.0, NOTE_FS3, 0.5, NOTE_G3, 1.5, NOTE_E3, 0.5, NOTE_CS3, 1.0,
    NOTE_D3, 1.5, NOTE_E3, 1.0, NOTE_A2, 0.5, NOTE_A2, 1.0,
    NOTE_FS3, 1.5, REST, 2.0,
     
    REST, 2.0, NOTE_G3, 0.5, NOTE_FS3, 0.5, NOTE_E3, 0.5, NOTE_E3, 0.5,
    NOTE_FS3, 2.0, REST, 2.0,
     
    // Third line
    REST, 1.0, REST, 0.5, NOTE_A2, 0.5, NOTE_G3, 0.5, NOTE_FS3, 0.5, NOTE_E3, 0.5, NOTE_E3, 1.5,
    NOTE_FS3, 0.5, NOTE_D3, 1.5, NOTE_E3, 0.5, NOTE_A2, 2.5,
    REST, 2.0,
     
    NOTE_E3, 1.0, NOTE_FS3, 0.5, NOTE_G3, 1.5, NOTE_E3, 0.5, NOTE_CS3, 1.5,
    NOTE_D3, 0.5, NOTE_E3, 1.0, NOTE_A2, 0.5, NOTE_D3, 0.5, NOTE_E3, 0.5,
     
    // Fourth line
    NOTE_F3, 0.5, NOTE_E3, 0.5, NOTE_D3, 0.5, NOTE_C3, 0.5, REST, 1.0, NOTE_A2, 0.5, NOTE_AS2, 0.5,
    NOTE_C3, 1.0, NOTE_F3, 1.0, NOTE_E3, 0.5, NOTE_D3, 0.5, NOTE_D3, 0.5, NOTE_C3, 0.5,
    NOTE_D3, 0.5, NOTE_C3, 0.5, NOTE_C3, 1.0, NOTE_C3, 1.0, NOTE_A2, 0.5, NOTE_AS2, 0.5,
    NOTE_C3, 1.0, NOTE_F3, 1.0, NOTE_G3, 0.5, NOTE_F3, 0.5, NOTE_E3, 0.5, NOTE_D3, 0.5,
    NOTE_D3, 0.5, NOTE_E3, 0.5, NOTE_F3, 1.0, NOTE_F3, 1.0, NOTE_G3, 0.5, NOTE_A3, 0.5,
     
    // Fifth line
    NOTE_AS3, 0.5, NOTE_AS3, 0.5, NOTE_A3, 1.0, NOTE_G3, 1.0, NOTE_F3, 0.5, NOTE_G3, 0.5,
    NOTE_A3, 0.5, NOTE_A3, 0.5, NOTE_G3, 1.0, NOTE_F3, 1.0, NOTE_D3, 0.5, NOTE_C3, 0.5,
    NOTE_D3, 0.5, NOTE_F3, 0.5, NOTE_F3, 0.5, NOTE_E3, 1.0, NOTE_E3, 0.5, NOTE_FS3, 0.5, NOTE_FS3, 1.5,
     
    
    // and belive me...
    REST, 2.0, NOTE_A3, 0.5, NOTE_A3, 0.5,
    NOTE_B3, 0.5, NOTE_A3, 0.5, NOTE_FS3, 0.5, NOTE_D3, 1.0, NOTE_E3, 0.5, NOTE_FS3, 0.5, NOTE_FS3, 1.5,
    REST, 1.5, NOTE_A3, 0.5, NOTE_A3, 0.5, NOTE_A3, 0.5,
     
    
    // science and im still...
    NOTE_B3, 0.5, NOTE_A3, 0.5, NOTE_FS3, 0.5, NOTE_D3, 1.0, NOTE_E3, 0.5, NOTE_FS3, 0.5, NOTE_FS3, 1.5,
     
    REST, 1.5, NOTE_A3, 0.5, NOTE_A3, 0.5, NOTE_A3, 0.5,
    NOTE_B3, 0.5, NOTE_A3, 0.5, NOTE_FS3, 0.5, NOTE_D3, 1.0, NOTE_E3, 0.5, NOTE_FS3, 0.5, NOTE_FS3, 1.5,
     
    REST, 2.0, NOTE_A3, 0.5, NOTE_A3, 0.5,
    NOTE_B3, 0.5, NOTE_A3, 0.5, NOTE_FS3, 0.5, NOTE_D3, 1.0, NOTE_E3, 0.5, NOTE_FS3, 0.5, NOTE_FS3, 1.5,
     
    REST, 1.5, NOTE_A3, 0.5, NOTE_A3, 0.5, NOTE_A3, 0.5,
    NOTE_B3, 0.5, NOTE_A3, 0.5, NOTE_FS3, 0.5, NOTE_D3, 1.0, NOTE_E3, 0.5, NOTE_FS3, 0.5, NOTE_FS3, 1.5,
     
    REST, 1.5, NOTE_A3, 0.5, NOTE_A3, 0.5, NOTE_A3, 0.5,
    NOTE_B3, 0.5, NOTE_A3, 0.5, NOTE_FS3, 0.5, NOTE_D3, 1.0, NOTE_E3, 0.5, NOTE_FS3, 0.5, NOTE_FS3, 1.5,
     
    // still alive... still alive...
    REST, 1.5, NOTE_G3, 0.5, NOTE_A3, 0.5, NOTE_A3, 1.5,
    REST, 1.5, NOTE_G3, 0.5, NOTE_FS3, 0.5, NOTE_FS3, 1.5,
     
    // End of song
    0.0, 0.0
};

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
     
    double duration = 1;
    for(int i = 0; duration > 0.0; i += 2) {
        int note = song[i];
        duration = song[i + 1];
         
        int noteDuration = (1600.0 * (duration / 4));
        tone(9, note, noteDuration);

        int pauseBetweenNotes = noteDuration * 1.3;
        delay(pauseBetweenNotes);
        noTone(9);
    }
}

void loop() {
    // no need to repeat the melody.
}
