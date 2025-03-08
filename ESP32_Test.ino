/// #include<Arduino.h>

const int outputPin1 = 12;
const int outputPin2 = 13;
const int inputPin1 = 4;
const int inputPin2 = 5;
void setup () {
    pinMode(outputPin1,OUTPUT); //Set pin to output
    pinMode(outputPin2,OUTPUT); 
    pinMode(inputPin1,INPUT); //Set pin to input
    pinMode(inputPin2,INPUT); 
}
void loop() {
    digitalWrite(outputPin, HIGH); //Send high signal
    delay(1000); //Delay a second
    digitalWrite(outputPin, LOW); //Send low signal
    delay(1000); //Delay a second
}

#include <esp_now.h>
#include <WiFi.h>


