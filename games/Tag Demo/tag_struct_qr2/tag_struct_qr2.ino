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

#define CODE1_CHECKSUM 0xD99E05C1
#define CODE_LENGTH 10
#define IMG_WIDTH 27
#define IMG_HEIGHT 27
const PROGMEM uint8_t img_data[] = {
    0x87, 0x47, 0x85, 0x7C, 0x7B, 0x93, 0x7F, 0x82,
    0xC5, 0xAD, 0x0D, 0x3E, 0xC1, 0xE0, 0x6B, 0x93,
    0xC5, 0xB3, 0x1F, 0xC1, 0x38, 0x82, 0x9B, 0x8A,
    0xF9, 0x27, 0x70, 0xAB, 0x58, 0xB9, 0x05, 0x44,
    0x67, 0x42, 0x77, 0xB9, 0x93, 0x6D, 0x86, 0x11,
    0x4B, 0x90, 0xBE, 0xA4, 0xCD, 0x5C, 0x09, 0xD1,
    0x42, 0x59, 0x73, 0x42, 0x97, 0x7B, 0xB1, 0xE1,
    0x29, 0x95, 0x16, 0x62, 0xD0, 0xFB, 0xE0, 0xB0,
    0x5F, 0xA1, 0xD5, 0xF8, 0xDB, 0x48, 0x0B, 0x4E,
    0x3F, 0x9C, 0x00, 0x09, 0x9F, 0x1C, 0xBF, 0xB2,
    0xF1, 0x25, 0x97, 0xCC, 0xE7, 0xE2, 0xA2, 0x20,
    0x7E, 0xFA, 0xEB, 0x4B, 0x91, 0x1E, 0x16, 0x31,
    0xD5, 0x76, 0xFF, 0x00, 0x90, 0x14, 0xA4, 0xDB,
    0x53, 0xB4, 0x52, 0x48
};


// The enemy pixel 'speed'
// This needs to be global because it's modified by the ISR.
float g_enemy_rate;


// Timer variables
// This needs to be global because it's modified by the ISR.
uint8_t g_ds = 0;
uint8_t g_sec = 0;
uint8_t g_min = 0;
char STRING_BUFFER[10];


uint32_t crc16(const uint8_t *data_p, const uint8_t length, const uint8_t start_offset = 0) {
    uint8_t x;
    uint16_t crc = 0xFFFF;
    uint16_t crc2 = 0;
    for(uint8_t i = 0; i < length; i++) {
        uint8_t idx = (start_offset + i) % length;
        uint8_t val = data_p[idx];
        crc2 = (crc2 + (val * i)) % 0xFFFF;
        x = crc >> 8 ^ val;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return ((uint32_t)crc << 16) | crc2;
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
        uint32_t checksum = crc16(key_history, CODE_LENGTH, index);
        switch(checksum) {
            case CODE1_CHECKSUM:
                timer_stop();
                const uint8_t scale_factor = 2;
                uint8_t bytes_per_row = ceil((double)IMG_WIDTH / 8.0);
                uint16_t offset = (checksum >> 16) & 0xFFFF;
                display.clearBuffer();
                for(uint8_t y = 0; y < IMG_HEIGHT; y++) {
                    for(uint8_t x = 0; x < bytes_per_row; x++) {
                        uint8_t i = x + (y * bytes_per_row);
                        uint8_t mod = (((uint32_t)key_history[(i + index) % CODE_LENGTH] * (uint32_t)i) + offset) % 0xFF;
                        uint8_t val = pgm_read_byte_near(img_data + i) ^ mod;
                        for(uint8_t xi = 0; xi < 8; xi++) {
                            if(xi + (x * 8) >= IMG_WIDTH)
                                break;
                            uint8_t x_pos = (xi * scale_factor) + (x * 8 * scale_factor);
                            uint8_t y_pos = y * scale_factor;
                            uint8_t bit = ((val >> xi) & 0x1);
                            display.setDrawColor(bit);
                            display.drawBox(x_pos + 37, y_pos + 5, scale_factor, scale_factor);
                        }
                    }
                }
                display.sendBuffer();
                delay(1000);
                display.setDrawColor(1);
                while(!key_fetch()) {}
                timer_start();
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
        
        display.sendBuffer();
        //delay(15);
    }
}
