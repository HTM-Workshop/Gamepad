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

// translated key inputs for key_fetch
#define KEY_LEFT    0x80
#define KEY_UP      0x40
#define KEY_B       0x08
#define KEY_A       0x04
#define KEY_RIGHT   0x02
#define KEY_DOWN    0x01

#define NUM_OF_ENEMIES 7

typedef struct {
    float x;
    float y;
} Pixel;

#define CODE1_CHECKSUM 0xD99E
#define CODE_LENGTH 10
#define MESSAGE_BUFFER_LEN 14
const uint8_t msg1[] = {
    0x74, 0x69, 0x6E, 0x79, 0x75, 0x72, 0x6C, 0x2E, 
    0x63, 0x6F, 0x6D, 0x2F
};
const uint8_t msg2[] = {
    0x3F, 0xD9, 0x17, 0x1E, 0x2A, 0xE3, 0x1F, 0xCD, 
    0xD7, 0xF2, 0x9B, 0x56, 0xED
};
uint8_t message_buffer[MESSAGE_BUFFER_LEN];



// The enemy pixel 'speed'
// This needs to be global because it's modified by the ISR.
float g_enemy_rate;


// Timer variables
// This needs to be global because it's modified by the ISR.
uint8_t g_ds = 0;
uint8_t g_sec = 0;
uint8_t g_min = 0;
char STRING_BUFFER[10];


uint16_t crc16(const uint8_t *data_p, const uint8_t length, const uint8_t start_offset = 0) {
    uint8_t x;
    uint16_t crc = 0xFFFF;
    for(uint8_t i = 0; i < length; i++) {
        uint8_t idx = (start_offset + i) % length;
        uint8_t val = data_p[idx];
        x = crc >> 8 ^ val;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}

uint8_t __attribute__((inline_always)) key_fetch() {

    // Fetch the raw input states from PORTD and PORTB
    uint8_t raw_input = 0x30;

    // Read the button states from the input ports
    raw_input |= (PIND & 0b11001100);  // PD7, PD6, PD3, PD2
    raw_input |= (PINB & 0b00000100) >> 1;  // PB2
    raw_input |= (PINB & 0b00000001);       // PB0

    // Invert the bits to get the active low representation
    return ~raw_input;
}


bool key_sequence_check(const uint8_t keycode) {
    static uint8_t key_history[CODE_LENGTH];
    static uint8_t index = 0;
    static bool key_held = false;
    static uint8_t chars_since_last_a = 0;
    if(key_held == true) {
        if(keycode == 0) {
            key_held = false;
            return false;
        } else {
            return false;
        }
    }
    if(keycode == 0) {
        return false;
    } else {
        key_held = true;
    }
    key_history[index] = keycode;
    index = (index + 1) % CODE_LENGTH;
    if(keycode == KEY_A) {
        if(chars_since_last_a < CODE_LENGTH - 1) {
            chars_since_last_a = 0;
            return false;
        }
        uint16_t checksum = crc16(key_history, CODE_LENGTH, index);
        switch(checksum) {
            case CODE1_CHECKSUM:
                uint8_t idx = index;
                for(uint8_t i = 0; i < sizeof(msg2); i++) {
                    message_buffer[i] = 0;
                    if(i >= MESSAGE_BUFFER_LEN)
                        break;
                    uint8_t mod = (((uint16_t)key_history[idx] * (uint16_t)i) + checksum) % 0xFF;
                    message_buffer[i] = msg2[i] ^ mod;
                    idx = (idx + 1) % CODE_LENGTH;
                }
                chars_since_last_a = 0;
                return true;
            default:
                chars_since_last_a = 0;
                return false;
        }
    } else {
        chars_since_last_a++;
        return false;
    }
    return false;
}


void timer_start() {
    TIMSK1 |= 1;
}


void timer_stop() {
    TIMSK1 &= 0;
}


// Measure time using interrupts for higher accruacy.
ISR(TIMER1_OVF_vect) {
    TCNT1 = 40535;
    g_ds++;
    if(g_ds > 9) {
        g_ds = 0;
        g_sec++;
        if(g_sec % 3 == 0 && g_enemy_rate < 2) {
            g_enemy_rate = g_enemy_rate + 0.1;
        }
        if(g_sec > 59) {
            g_sec = 0;
            g_min++;
        }
    }
}


void setup() {
    display.begin();
    display.setFont(u8g2_font_profont11_tr);
    randomSeed(analogRead(A6));

    // Timing interrupts
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 40535;                      // 65535 - 40535 = 25000
    TCCR1B |= _BV(CS11) | _BV(CS10);    // 16MHz / 64 = 250000hz
                                        // 250000hz / 25000 = 10hz = 0.10 seconds
}


void loop() {

    // Store character height offset
    const uint8_t line_offset = display.getAscent() + 1;

    // player position
    float player_x = 60;
    float player_y = 20;
    const float player_speed = 2;

    // create enemies
    Pixel enemies[NUM_OF_ENEMIES];
    for(uint8_t i = 0; i < NUM_OF_ENEMIES; i++) {
        enemies[i].x = random(0, SCREEN_WIDTH);
        enemies[i].y = random(0, SCREEN_HEIGHT);
    }

    // initial speed
    g_enemy_rate = 0.1;

    // size of pixels
    const uint8_t pixel_size = 2;

    // reset timer
    g_ds = 0;
    g_sec = 0;
    g_min = 0;

    // begin game
    bool game_running = true;
    timer_start();
    uint8_t message_reveal = false;
    
    // main loop
    while(1) {
        
        // clear dispay
        display.clearBuffer();
        
        // == Handle Inputs ==
        // if the game is running
        if(game_running == true) {
            if(!digitalRead(BUTTON_DOWN) && player_y < SCREEN_HEIGHT - pixel_size) {
                player_y += player_speed;
            }
            if(!digitalRead(BUTTON_UP) && player_y > 0) {
                player_y -= player_speed;
            }
            if(!digitalRead(BUTTON_RIGHT) && player_x < SCREEN_WIDTH - pixel_size) {
                player_x += player_speed;
            }
            if(!digitalRead(BUTTON_LEFT) && player_x > 0) {
                player_x -= player_speed;
            }
        } else {
            if(!digitalRead(BUTTON_A)) {
                break;      // exits the while loop and goes back to the loop() function.
            }
        }
        uint8_t code_status = key_sequence_check(key_fetch());
        if(code_status)
            message_reveal = 80;


        // == Update Game Logic ==
        if(game_running == true) {
            
            // update enemy positions
            for(uint8_t i = 0; i < NUM_OF_ENEMIES; i++) {
                if(player_x > enemies[i].x) {
                    enemies[i].x = enemies[i].x + g_enemy_rate;
                } else if(player_x < enemies[i].x) {
                    enemies[i].x = enemies[i].x - g_enemy_rate;
                }
                if(player_y > enemies[i].y) {
                    enemies[i].y = enemies[i].y + g_enemy_rate;
                } else if(player_y < enemies[i].y) {
                    enemies[i].y = enemies[i].y - g_enemy_rate;
                }

                // check if this enemy has caught the player
                if(abs(player_x - enemies[i].x) < g_enemy_rate &&
                   abs(player_y - enemies[i].y) < g_enemy_rate) {
                    game_running = false;
                    timer_stop();
                    break;
                }
            }
        }


        // == Update Display ==

        // draw the player pixel
        display.drawBox(floor(player_x), floor(player_y), pixel_size, pixel_size);

        // draw enemy pixels
        for(uint8_t i = 0; i < NUM_OF_ENEMIES; i++) {
            display.drawBox(floor(enemies[i].x), floor(enemies[i].y), pixel_size, pixel_size);
        }


        // print the status bar
        sprintf(STRING_BUFFER, "%02d:%02d:%d ", g_min, g_sec, g_ds);
        display.setCursor(5, line_offset * 1);
        display.print(STRING_BUFFER);
        display.print("SPEED:");
        display.print(g_enemy_rate);

        // if the game is over, show this message
        if(game_running == false) {
            display.drawStr(35, line_offset * 3, "CAUGHT!");
            display.drawStr(0, line_offset * 4, "Press A to try again");
        }

        if(message_reveal > 0) {
            display.drawStr(2, line_offset * 6, msg1);
            display.drawStr(2, line_offset * 7, message_buffer);
            message_reveal -= 1;
        }
        
        display.sendBuffer();
        //delay(15);
    }
}
