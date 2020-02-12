#include "Arduino.h"
#include "config.h"

byte lightPins[] = {
    LT_START, 
    LT_A, 
    LT_B, 
    LT_C, 
    LT_D, 
    LT_FX_L, 
    LT_FX_R
};
byte lastControl;
byte controls;
byte response[RESPONSE_SIZE];
int buttonUpdateSend;
int volL;
int volR;

void setup() {

    buttonUpdateSend = 0;

    // BT A
    pinMode(BT_A, INPUT);
    digitalWrite(BT_A, HIGH);

    // BT B
    pinMode(BT_B, INPUT);
    digitalWrite(BT_B, HIGH);

    // BT C
    pinMode(BT_C, INPUT);
    digitalWrite(BT_C, HIGH);

    // BT D
    pinMode(BT_D, INPUT);
    digitalWrite(BT_D, HIGH);

    // FX L
    pinMode(BT_FX_L, INPUT);
    digitalWrite(BT_FX_L, HIGH);

    // FX R
    pinMode(BT_FX_R, INPUT);
    digitalWrite(BT_FX_R, HIGH);

    // START
    pinMode(BT_START, INPUT);
    digitalWrite(BT_START, HIGH);

    // TEST
    pinMode(BT_TEST, INPUT);
    digitalWrite(BT_TEST, HIGH);

    // SERVICE
    pinMode(BT_SERVICE, INPUT);
    digitalWrite(BT_SERVICE, HIGH);

    // Init lights
    int i;
    for (i = 0; i < 7; i++) pinMode(lightPins[i], OUTPUT); 

    Serial.begin(57600);
    while (!Serial) {}
    setLights(255, 255, 255);

}

void loop() {

    buttonUpdateSend++;
    if (buttonUpdateSend > CONTROL_UPDATES) {

        response[0] = 0xAA;
        getControls();
        getEncoders();
        Serial.write(response, RESPONSE_SIZE);
        buttonUpdateSend = 0;

    }

}

void getEncoders() {

    volL = analogRead(VOL_L);
    volR = analogRead(VOL_R);

    response[2] = (volL >> 24) & 0xFF;
    response[3] = (volL >> 16) & 0xFF;
    response[4] = (volL >> 8) & 0xFF;
    response[5] = volL & 0xFF;

    response[6] = (volR >> 24) & 0xFF;
    response[7] = (volR >> 16) & 0xFF;
    response[8] = (volR >> 8) & 0xFF;
    response[9] = volR & 0xFF;
  
}

void getControls() {
  
    controls = 0x0;
    if (!digitalRead(BT_A))
        controls |= 0x1;
    if (!digitalRead(BT_B))
        controls |= 0x2;
    if (!digitalRead(BT_C))
        controls |= 0x4;
    if (!digitalRead(BT_D))
        controls |= 0x8;
    if (!digitalRead(BT_FX_L))
        controls |= 0x10;
    if (!digitalRead(BT_FX_R))
        controls |= 0x20;
    if (!digitalRead(BT_START))
        controls |= 0x40;
    if (!digitalRead(BT_TEST))
        controls |= 0x80;
        
    response[1] = controls;
  
}

void setLights(unsigned char r, unsigned char g, unsigned char b) {

    analogWrite(LT_CON_R, 255 - r);
    analogWrite(LT_CON_B, 255 - b);
    analogWrite(LT_CON_G, 255 - g);

}
