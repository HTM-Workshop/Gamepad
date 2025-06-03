/*
    This is the full framebuffer version of the example template.
    It's less complicated to use, but uses a significant amount
    of RAM.
*/

#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>

// OLED Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


// Pin mapping for Version 2.1 of Gamepad
#define BUTTON_A 2
#define BUTTON_B 3
#define BUTTON_UP 6
#define BUTTON_LEFT 7
#define BUTTON_RIGHT 10
#define BUTTON_DOWN 8
#define BUZZER 9
#define LED_LEFT 4
#define LED_RIGHT 5



void setup() {
    display.begin();
    display.setFont(u8g2_font_profont17_tr);
    
}


void loop() {

    // main loop
    while(1) {
        
        // clear dispay
        display.clearBuffer();
        
        // draw functions go here
        
        // update display
        display.sendBuffer();
        delay(50);
    }
}
