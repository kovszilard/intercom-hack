#include <TMRpcm.h>
#include <SD.h>

// 1 means long keypress, 0 means short keypress
const int goodCode[] = {0, 0, 0, 0};

const int BELL_INPUT_PIN = 2;
const int GATE_OPEN_PIN = 7;
const int SOUND_OUT_PIN = 9;
const int SD_CS_PIN = 4;

TMRpcm tmrpcm;


void setup() {
  pinMode(BELL_INPUT_PIN,INPUT);
  pinMode(GATE_OPEN_PIN, OUTPUT);
  digitalWrite(GATE_OPEN_PIN, HIGH);

  tmrpcm.speakerPin = SOUND_OUT_PIN;

  Serial.begin(9600);
  
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

}

int checkButtonPress(void (*onShortPress)(), void (*onLongPress)(), int buttonPin) {

  const int checkInterval = 100;
  const int longPressIntervalMultiplier = 4;
  
  if (digitalRead(buttonPin) == LOW) {

    int counter = 1;
    delay(checkInterval);

    while(digitalRead(buttonPin) == LOW) {
      
      counter++;
      if (counter >= longPressIntervalMultiplier) {
        //Serial.println("Button press will be considered a long press...");
      }
      delay(checkInterval);
    }

    if (counter >= longPressIntervalMultiplier) {
      onLongPress();
      return 1;
    } else {
      onShortPress();
      return 0;
    }
  } else return -1;
}

void shortPress () {}

void longPress () {}

void openGate() {
  digitalWrite(GATE_OPEN_PIN, LOW);
  delay(3000);
  digitalWrite(GATE_OPEN_PIN, HIGH);
}

void playAccesGrantedSound() {
  tmrpcm.play("granted.wav");
}

void playAccessDeniedSound() {
  tmrpcm.play("denied.wav");
}

int givenCodes[] = {-1, -1, -1, -1};

void loop() {

  bool timeOut = false;
  
  int state = checkButtonPress(shortPress, longPress, BELL_INPUT_PIN);

  // reading 1st code
  if (state != -1) {
    givenCodes[0] = state;
    long code1registeredAt = millis();
    Serial.print("[");
    Serial.print(state);

    // reading 2nd code
    while (millis() - code1registeredAt < 2000) {

      state = checkButtonPress(shortPress, longPress, BELL_INPUT_PIN);
      if (state != -1) {
        givenCodes[1] = state;
        long code2registeredAt = millis();
        Serial.print(state);

        // reading 3rd code
        while (millis() - code2registeredAt < 2000) {
    
          state = checkButtonPress(shortPress, longPress, BELL_INPUT_PIN);
          if (state != -1) {
            givenCodes[2] = state;
            long code3registeredAt = millis();
            Serial.print(state);

            // reading 4th code
            while (millis() - code3registeredAt < 2000) {
        
              state = checkButtonPress(shortPress, longPress, BELL_INPUT_PIN);
              if (state != -1) {
                givenCodes[3] = state;
                Serial.print(state);
                Serial.print("] - ");

                if (givenCodes[0] == goodCode[0] &&
                  givenCodes[1] == goodCode[1] &&
                  givenCodes[2] == goodCode[2] &&
                  givenCodes[3] == goodCode[3]) {
                    Serial.println("GOOD!");
                    playAccesGrantedSound();
                    delay(1000);
                    openGate();
                    return;
                  } else {
                    Serial.println("WRONG!");
                    playAccessDeniedSound();
                    return;
                  }
              }
            }
          }
        }
      }
    }
    Serial.println(" - TIMEOUT!");
  }
  
  
  
}
