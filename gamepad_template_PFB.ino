/*
    This is the paged framebuffer version of the example template.
    It is somewhat more complicated to use, but saves a significant
    amount of RAM.
*/

#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>

// OLED Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


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

        // update display
        display.firstPage();
        do {
            
        } while(display.nextPage());
    
}
