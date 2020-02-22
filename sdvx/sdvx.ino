#include "Arduino.h"
#include "config.h"

byte lightPins[] = { 
    LT_A, 
    LT_B, 
    LT_C, 
    LT_D, 
    LT_FX_L, 
    LT_FX_R,
    LT_START
};
byte lastControl;
byte controls;
byte response[RESPONSE_SIZE];
int buttonUpdateSend;
int volL;
int volR;

char colPins[] = {K1_A, K1_B, K1_C};
char rowPins[] = {K1_1, K1_2, K1_3, K1_4};
int keypad;

#ifdef RAINLIGHT
int r = 0;
int g = 0;
int b = 0;
int colors[6][3] = {
    255, 0, 0,
    0, 255, 0,
    0, 0, 255,
    255, 255, 0,
    80, 0, 80,
    0, 255, 255
};
int curR;
int curG;
int curB;
int curColor = 0;
int lastColorUpdate = 0;
#endif

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

    // Init Reader
    for (i = 0; i < 3; i++) {
        pinMode(colPins[i], INPUT);
    }

    for (i = 0; i < 4; i++) {
        pinMode(rowPins[i], INPUT);
        digitalWrite(rowPins[i], HIGH);
    }
    

    Serial.begin(57600);
    while (!Serial) {}
    setConLights(255, 255, 255);

}

void loop() {

    if (Serial.available() > 0) Serial.flush();

    readKeypad();
    if (keypad != 0x0) Serial.write(keypad);
    delay(100);

    buttonUpdateSend++;
    if (buttonUpdateSend > CONTROL_UPDATES) {

        response[0] = 0xAA;
        getControls();
        getEncoders();
        buttonLightUpdate();
        //Serial.write(response, RESPONSE_SIZE);
        buttonUpdateSend = 0;

    }

#ifdef RAINLIGHT
    lastColorUpdate++;
    if (lastColorUpdate > 300) {

        updateLights();
        lastColorUpdate = 0;

    }
#endif

}

void readKeypad() {

    keypad = 0x0;
    int x;
    int y;
    for (x = 0; x < 3; x++) {

        pinMode(colPins[x], OUTPUT);
        digitalWrite(colPins[x], LOW);

        for (y = 0; y < 4; y++) {

            if (!digitalRead(rowPins[y])) {

                keypad |= (0x1 << (y + x * 4));
              
            }
          
        }
        pinMode(colPins[x], INPUT);
      
    }
  
}

#ifdef RAINLIGHT
void updateLights() {

    curR = colors[curColor][0];
    curG = colors[curColor][1];
    curB = colors[curColor][2];
    if ((curR == r) || (curG == g) || (curB == b)) curColor++;
    if (curColor > 6) curColor = 0;

    if (r < curR) r += 1;
    if (r > curR) r -= 1;

    if (g < curG) g += 1;
    if (g > curG) g -= 1;

    if (b < curB) b += 1;
    if (b > curB) b -= 1;

    setConLights(r, g, b);
    
}
#endif

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

void setConLights(unsigned char r, unsigned char g, unsigned char b) {

    analogWrite(LT_CON_R, 255 - r);
    analogWrite(LT_CON_B, 255 - b);
    analogWrite(LT_CON_G, 255 - g);

}

void buttonLightUpdate() {

    int i;
    int x;
    int toSet;
    for (i = 0; i < 7; i++) {

        x = 1 << i;
        toSet = ((response[1] & x) != 0) ? 1 : 0;
        digitalWrite(lightPins[i], toSet);

    }

}
