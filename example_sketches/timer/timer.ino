
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


uint8_t G_SEC = 26;
uint8_t G_MIN = 0;
uint8_t G_HRS = 0;
char STRING_BUFFER[10];


void timer_run(void) {
    uint16_t sum = 1;
    while(sum > 0) {
        sum = G_SEC + G_MIN + G_HRS;
        sprintf(STRING_BUFFER, "%02d:%02d:%02d", G_HRS, G_MIN, G_SEC);
        display.clearDisplay();
        display.setCursor(0, 0); 
        display.println(STRING_BUFFER);
        display.display();
        delay(50);
    }

    while(true) {
        digitalWrite(LED_LEFT, HIGH);
        digitalWrite(LED_RIGHT, HIGH);
        delay(500);
        digitalWrite(LED_LEFT, LOW);
        digitalWrite(LED_RIGHT, LOW);
        delay(500);
    }
}


void setup() {
    sprintf(STRING_BUFFER, "%02d:%02d:%02d", G_HRS, G_MIN, G_SEC);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.setCursor(0, 0); 
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println(STRING_BUFFER);
    display.display();

    // pins
    pinMode(LED_LEFT, OUTPUT);
    pinMode(LED_RIGHT, OUTPUT);


    // setup timers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1H = 0x0B;
    TCNT1L = 0xDC;
    TIMSK1 |= _BV(TOIE1);
    TCCR1B |= _BV(CS12);

    timer_run();
}


ISR(TIMER1_OVF_vect) {
    TCNT1H = 0x0B;
    TCNT1L = 0xDC;
    G_SEC = (G_SEC - 1);
    if(G_SEC == 255) {
        G_MIN = (G_MIN - 1);
        G_SEC = 59;
        if(G_MIN == 255) {
            G_HRS = (G_HRS - 1);
            G_MIN = 59;
        }
    }
}


void loop() {

    
}
