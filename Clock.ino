#include <Ticker.h>

Ticker ticker;

volatile int seconds = 0; // second
volatile int minutes = 55; // minute
volatile int hours = 12; // hour
int digits[4]; // digits to be displayed

const byte TABLE[10] = {
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2 
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5 
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110  // 9
};

// Pin Definitions
const int CATHODE_PINS[] = {22, 21, 19, 4}; // pins for display
const int LATCH_PIN = 5; // latch pin
const int CLOCK_PIN = 18; // clock pin
const int DATA_PIN = 23; // data pin to shift register
const int DISPLAY_SWITCH_PIN = 13; // D1 pin for display switch (GPIO 1)

bool displayOn = true; // Flag to track display state

void incrementSeconds() {
  // increment seconds on software timer
  seconds++;
  if (seconds >= 60) {
    seconds = 0;
    incrementMinutes();
  }
  Serial.print("Seconds: ");
  Serial.println(seconds);
}

void incrementMinutes() {
  // increments minutes as needed
  minutes++;
  if (minutes >= 60) {
    minutes = 0;
    incrementHours();
  }
  updateDisplay();
  Serial.print("Minutes: ");
  Serial.println(minutes);
}

void incrementHours() {
  // increment hours as needed
  hours++;
  if (hours >= 24) {
    hours = 0;
  }
  Serial.print("Hours: ");
  Serial.println(hours);
}

void screenOff() { 
  // reset display
  for (int i = 0; i < 4; i++) {
    digitalWrite(CATHODE_PINS[i], HIGH);
  }
}

void separate() {
  // set values in digits[] to be in their locations
  int min = minutes;
  int hr = hours;
  digits[0] = hr / 10;
  digits[1] = hr % 10;
  digits[2] = min / 10;
  digits[3] = min % 10;

  // Debugging information
  Serial.print("Separated Digits: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(digits[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void updateDisplay() {
  // update the display with the current time
  separate(); // separate the hours and minutes into digits array
}

void showDigit(int digitIndex) {
  digitalWrite(CATHODE_PINS[digitIndex], LOW); // turn on the current digit
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, TABLE[digits[digitIndex]]); // send the data to the shift register
  digitalWrite(LATCH_PIN, HIGH); // latch the data
  digitalWrite(LATCH_PIN, LOW); // reset the latch
  delay(5); // Small delay to avoid ghosting
  digitalWrite(CATHODE_PINS[digitIndex], HIGH); // turn off the current digit
}

void setup() {
  Serial.begin(115200);
  // Initialize pins
  for (int i = 0; i < 4; i++) {
    pinMode(CATHODE_PINS[i], OUTPUT);
    digitalWrite(CATHODE_PINS[i], HIGH);
  }
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(DISPLAY_SWITCH_PIN, INPUT_PULLUP); // set display switch pin as input with internal pull-up resistor

  ticker.attach(1.0, incrementSeconds); // Call incrementSeconds every second
}

void loop() {
  // Check display switch state
  if (digitalRead(DISPLAY_SWITCH_PIN) == LOW) {
    // Switch pressed (display OFF)
    if (displayOn) {
      displayOn = false;
      screenOff(); // turn off the display
      Serial.println("Display OFF");
    }
  } else {
    // Switch not pressed (display ON)
    if (!displayOn) {
      displayOn = true;
      Serial.println("Display ON");
    }
    updateDisplay(); // update the display with current time
    for (int i = 0; i < 4; i++) {
      showDigit(i); // show each digit on the display
    }
  }
}

