
/* Example Template */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// Pin mapping for Version 2.1 of Gamepad
#define BUTTON_A 2
#define BUTTON_B 3
#define BUTTON_UP 6
#define BUTTON_LEFT 7
#define BUTTON_RIGHT 10
#define BUTTON_DOWN 9
#define BUZZER 8
#define LED_LEFT 4
#define LED_RIGHT 5



// Stopwatch variables
uint8_t G_MS = 0;
uint8_t G_SEC = 0;
uint8_t G_MIN = 0;
char STRING_BUFFER[10];



// Measure time using interrupts for higher accruacy.
ISR(TIMER1_OVF_vect) {
    TCNT1 = 40535;
    G_MS++;
    if(G_MS > 9) {
        G_MS = 0;
        G_SEC++;
        if(G_SEC > 59) {
            G_SEC = 0;
            G_MIN++;
        }
    }
}


void setup() {

    // Initialize the OLED panel
    sprintf(STRING_BUFFER, "%02d:%02d:%02d", G_MIN, G_SEC, G_MS);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay();

    // Timer interrupts. See ATMEGA328P datasheet for details.
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 40535;
    TCCR1B |= _BV(CS11) | _BV(CS10);
    TIMSK1 &= 0;

}


void loop() {
    bool button_held = false;
    bool timer_running = false;
    
    // Main Loop
    while(1) {
        
        // Finite state machine
        if(button_held) {
            if(digitalRead(BUTTON_A) && digitalRead(BUTTON_B))
                button_held = false;
        } else {
            if(!digitalRead(BUTTON_A)) {
                if(timer_running) {
                    timer_running = false;
                    TIMSK1 &= 0;
                } else {
                    timer_running = true;
                    TIMSK1 |= _BV(TOIE1);
                }
                button_held = true;
            }
            if(!digitalRead(BUTTON_B)) {
                G_MS = 0;
                G_SEC = 0;
                G_MIN = 0;
                button_held = true;
            }
        }
        
        // update display
        sprintf(STRING_BUFFER, "%02d:%02d:%02d", G_MIN, G_SEC, G_MS);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(STRING_BUFFER);
        display.println("A = Start/Stop\nB = Reset");
        display.display();
        delay(50);
    }
}
