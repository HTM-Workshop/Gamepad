#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3

#define LED_LEFT 4
#define LED_RIGHT 5



void setup() {
    pinMode(LED_LEFT, OUTPUT);
    pinMode(LED_RIGHT, OUTPUT);
}

void loop() {
    if(!digitalRead(BUTTON_LEFT)) {
        digitalWrite(LED_LEFT, HIGH);
        digitalWrite(LED_RIGHT, LOW);
    }
    if(!digitalRead(BUTTON_RIGHT)) {
        digitalWrite(LED_LEFT, LOW);
        digitalWrite(LED_RIGHT, HIGH);
    }
}
