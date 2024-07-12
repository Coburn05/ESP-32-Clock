wiring diagram I used can be found at this link.
https://microcontrollerslab.com/wp-content/uploads/2021/12/ESP32-with-74HC595-and-4-digit-7-segment.jpg
Switch is added to ground and pin 13

Rotary Encoder is added to connect DT pin to the D2 pin on the ESP and CLK pin to the D15 pin on the ESP.
It is used for time adjustments.

The BLE_Clock file can be run along with the Mac app found here: https://github.com/Coburn05/ESP32-BLE
this allows for time adjustments to be made over a BLE connection
