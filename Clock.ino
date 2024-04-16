#include "Timer.h"
Timer timer;

// Pin Definitions
const int CATHODE_PINS[] = {22, 21, 19, 4};
const int LATCH_PIN = 5;
const int CLOCK_PIN = 18;
const int DATA_PIN = 23;
const int ENCODER_PIN_DT = 2; 
const int ENCODER_PIN_CLK = 15; 
const int ENCODER_BUTTON_PIN = 14; 

// Display Data
const byte TABLE[10] = {
  B11111100, B01100000, B11011010, B11110010, B01100110,
  B10110110, B10111110, B11100000, B11111110, B11110110
};

// Initial Time Setup
int startHour = 3;
int startMinute = 55;

// State Variables
int count = 0;
int numbers[4];
int lastStateCLK = LOW; // Added initialization for lastStateCLK
unsigned long lastAdjustmentTime = 0; // Added initialization for lastAdjustmentTime
bool timerSuspended = false; // Added initialization for timerSuspended

void setup() {
  Serial.begin(115200); 
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(CATHODE_PINS[i], OUTPUT);
    digitalWrite(CATHODE_PINS[i], HIGH);
  }
  pinMode(ENCODER_PIN_DT, INPUT_PULLUP);
  pinMode(ENCODER_PIN_CLK, INPUT_PULLUP);
}

void loop() {
  timer.update(); 

  unsigned long currentTime = millis() / 1000;
  int elapsedMinutes = (currentTime / 60) + startMinute;
  int elapsedHours = (currentTime / 3600) + startHour;
  if (elapsedMinutes >= 60) {
    elapsedMinutes -= 60;
    elapsedHours++;
  }
  if (elapsedHours >= 24) {
    elapsedHours -= 24;
  }
  
  adjustTime();
  separate(elapsedHours, elapsedMinutes);
  displayTime();

  if (timerSuspended && (currentTime - lastAdjustmentTime >= 5)) {
    timerSuspended = false;
  }
}

void separate(int hours, int minutes) {
  numbers[0] = hours / 10;
  numbers[1] = hours % 10;
  numbers[2] = minutes / 10;
  numbers[3] = minutes % 10;
}

void displayTime() {
  screenOff(); 
  digitalWrite(LATCH_PIN, LOW); 
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, TABLE[numbers[count]]); 
  digitalWrite(CATHODE_PINS[count], LOW); 
  digitalWrite(LATCH_PIN, HIGH); 
  count++; 
  if (count == 4) { 
    count = 0;
  }
}

void screenOff() { 
  for (int i = 0; i < 4; i++) {
    digitalWrite(CATHODE_PINS[i], HIGH);
  }
}

void adjustTime() {
  int currentStateCLK = digitalRead(ENCODER_PIN_CLK);
  int currentStateDT = digitalRead(ENCODER_PIN_DT);

  if (currentStateCLK != lastStateCLK && currentStateCLK == HIGH) {
    if (currentStateDT != currentStateCLK) {
      incrementMinute();
    } else {
      decrementMinute();
    }

    lastAdjustmentTime = millis() / 1000;
    timerSuspended = true;
  }

  lastStateCLK = currentStateCLK;
}

void incrementMinute() {
  startMinute++;
  if (startMinute >= 60) {
    startMinute = 0;
    startHour++;
    if (startHour >= 24) {
      startHour = 0;
    }
  }
}

void decrementMinute() {
  startMinute--;
  if (startMinute < 0) {
    startMinute = 59;
    startHour--;
    if (startHour < 0) {
      startHour = 23;
    }
  }
}
