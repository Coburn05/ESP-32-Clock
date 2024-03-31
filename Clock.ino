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

// Set the start time to 3:55
int startHour = 13;
int startMinute = 47;

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
  Serial.println("Displaying elapsed time...");
}

void loop() {
  timer.update(); 

  // Get the current time in seconds since the Arduino board started
  unsigned long currentTime = millis() / 1000;

  // Convert seconds to minutes and seconds
  unsigned long elapsedSeconds = currentTime;
  unsigned long startSeconds = startHour * 3600 + startMinute * 60;
  unsigned long totalElapsedSeconds = elapsedSeconds + startSeconds;
  unsigned long elapsedMinutes = totalElapsedSeconds / 60;
  unsigned long elapsedHours = elapsedMinutes / 60;

  // Calculate the remaining minutes after subtracting full hours
  elapsedMinutes %= 60;

  // Handle overflow of hours
  elapsedHours %= 24;

  // Separate hours and minutes into individual digits
  separate(elapsedHours, elapsedMinutes);

  // Display the time
  Display();
}


void separate(int hours, int minutes) {
  num1 = hours / 10;
  numbers[0] = num1;
  num2 = hours % 10;
  numbers[1] = num2;
  num3 = minutes / 10;
  numbers[2] = num3;
  num4 = minutes % 10;
  numbers[3] = num4;
}

void Display() {
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
