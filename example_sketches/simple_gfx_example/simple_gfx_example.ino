
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

uint8_t x = 0;
uint8_t y = 0;

void setup() {
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    
}

void loop() {

    
}
