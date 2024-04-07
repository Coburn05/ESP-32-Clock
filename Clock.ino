#include "Timer.h"
Timer timer;

int num1 = 0;
int num2 = 0;
int num3 = 0;
int num4 = 0;

int D1 = 22;
int D2 = 21;
int D3 = 19;
int D4 = 4;

int latchPin = 5;
int clockPin = 18;
int dataPin = 23;

int count = 0;
int numbers[4];
int cathodePins[] = {22, 21, 19, 4};

byte table[10] = {B11111100, B01100000, B11011010, B11110010, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110};

// Set the start time to 3:55 PM
int startHour = 3;
int startMinute = 55;

// Rotary encoder pins
int encoderPinDT = 2; // Connected to D2 on ESP32
int encoderPinCLK = 15; // Connected to D15 on ESP32
int encoderButtonPin = 14; // Example pin for the button, change as needed

// Variables to track encoder rotation
int lastStateCLK = LOW;
int currentStateCLK;
int lastStateDT = LOW;
int currentStateDT;

// Variable to track the last adjustment time
unsigned long lastAdjustmentTime = 0;

// Variable to indicate if timer is suspended
bool timerSuspended = false;

void setup() {
  Serial.begin(115200); 
  pinMode(D4, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(D4, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D1, HIGH);

  // Set encoder pins as inputs
  pinMode(encoderPinDT, INPUT_PULLUP);
  pinMode(encoderPinCLK, INPUT_PULLUP);
}

void loop() {
  timer.update(); 

  // Get the current time in seconds since the Arduino board started
  unsigned long currentTime = millis() / 1000;

  // Convert seconds to minutes and hours
  int elapsedMinutes = (currentTime / 60) + startMinute;
  int elapsedHours = (currentTime / 3600) + startHour;

  // Handle overflow of minutes and hours
  if (elapsedMinutes >= 60) {
    elapsedMinutes -= 60;
    elapsedHours++;
  }
  if (elapsedHours >= 24) {
    elapsedHours -= 24;
  }

  // Adjust time based on encoder input
  adjustTime();

  // Separate hours and minutes into individual digits
  separate(elapsedHours, elapsedMinutes);

  // Display the time
  displayTime();

  // Check if the timer needs to be resumed
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
  digitalWrite(latchPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, table[numbers[count]]); 
  digitalWrite(cathodePins[count], LOW); 
  digitalWrite(latchPin, HIGH); 
  count++; 
  if (count == 4) { 
    count = 0;
  }
}

void screenOff() { 
  digitalWrite(D4, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D1, HIGH);
}

void adjustTime() {
  // Read the current state of the encoder pins
  currentStateCLK = digitalRead(encoderPinCLK);
  currentStateDT = digitalRead(encoderPinDT);

  // If the CLK pin has changed state
  if (currentStateCLK != lastStateCLK && currentStateCLK == HIGH) {
    // If the DT pin is different from the CLK pin, the encoder is being turned clockwise
    if (currentStateDT != currentStateCLK) {
      incrementMinute();
    } else { // Otherwise, the encoder is being turned counterclockwise
      decrementMinute();
    }

    // Record the time of adjustment
    lastAdjustmentTime = millis() / 1000;

    // Suspend the timer
    timerSuspended = true;
  }

  // Update last CLK pin state
  lastStateCLK = currentStateCLK;
}

void incrementMinute() {
  startMinute++;
  if (startMinute >= 60) {
    startMinute = 0;
    startHour++;
    if (startHour >= 24) {
      startHour = 0; // Reset to 0 when 24 hours reached
    }
  }
}

void decrementMinute() {
  startMinute--;
  if (startMinute < 0) {
    startMinute = 59; // Reset to 59 when negative
    startHour--;
    if (startHour < 0) {
      startHour = 23; // Wrap around to 23 when reaching -1
    }
  }
}

