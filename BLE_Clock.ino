#include <Ticker.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

Ticker ticker;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool pairingMode = false; // Flag to track pairing mode state
String receivedValue;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

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
const int DISPLAY_SWITCH_PIN = 13; // D13 pin for display switch (GPIO 13)

bool displayOn = true; // Flag to track display state

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String value = pCharacteristic->getValue().c_str();
        if (value.length() > 0) {
            receivedValue = value.c_str();
            Serial.print("Received Value: ");
            Serial.println(receivedValue);
            if (receivedValue.length() == 4) {
                int newHours = receivedValue.substring(0, 2).toInt();
                int newMinutes = receivedValue.substring(2, 4).toInt();
                if (newHours >= 0 && newHours < 24 && newMinutes >= 0 && newMinutes < 60) {
                    hours = newHours;
                    minutes = newMinutes;
                    seconds = 0; // Reset seconds
                    Serial.print("Time set to: ");
                    Serial.print(hours);
                    Serial.print(":");
                    Serial.println(minutes);
                } else {
                    Serial.println("Invalid time format");
                }
            }
        }
    }
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Device connected");
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected");
    }
};

void incrementSeconds() {
    // increment seconds on software timer
    seconds++;
    if (seconds >= 60) {
        seconds = 0;
        incrementMinutes();
    }
}

void incrementMinutes() {
    // increments minutes as needed
    minutes++;
    if (minutes >= 60) {
        minutes = 0;
        incrementHours();
    }
    updateDisplay();
}

void incrementHours() {
    // increment hours as needed
    hours++;
    if (hours >= 24) {
        hours = 0;
    }
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

    BLEDevice::init("CLOCK BLE TEST");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Waiting for a client connection to notify...");

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
        // Switch pressed (enter pairing mode)
        if (!pairingMode) {
            pairingMode = true;
            BLEDevice::startAdvertising();
            Serial.println("Entering Pairing Mode...");
        }
    } else {
        // Switch not pressed (exit pairing mode)
        if (pairingMode) {
            pairingMode = false;
            BLEDevice::stopAdvertising();
            Serial.println("Exiting Pairing Mode...");
        }

        updateDisplay(); // update the display with current time
        for (int i = 0; i < 4; i++) {
            showDigit(i); // show each digit on the display
        }
    }
}
