#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xcc, 0x8d, 0xa2, 0x0f, 0xc9, 0x84};

// Structure to send data
typedef struct struct_message {
  int b;  // Value to send
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// Define button pins
const int buttonPin4 = 4;
const int buttonPin5 = 5;

// Define LED pins (corresponding to values 0-6)
const int ledPins[7] = {18, 8, 3, 46, 9, 10, 11};

// Variable to hold the value
int value = 0;

// Variables to track button states
bool lastButton4State = LOW;
bool lastButton5State = LOW;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 5;

// **Non-blocking send timing**
unsigned long lastSendTime = 0;  // Last time data was sent
const unsigned long sendInterval = 5;  // Adjust send interval (milliseconds)

// Callback when data is sent (optional)
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register Send Callback (optional)
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Set button pins as inputs with internal pull-down resistors
  pinMode(buttonPin4, INPUT_PULLDOWN);
  pinMode(buttonPin5, INPUT_PULLDOWN);

  // Set LED pins as outputs
  for (int i = 0; i < 7; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);  // Ensure all LEDs are off initially
  }
}

void loop() {
  // Read the current state of the buttons
  bool button4State = digitalRead(buttonPin4) == HIGH;
  bool button5State = digitalRead(buttonPin5) == HIGH;

  // Check if enough time has passed since the last button press (debouncing)
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    // If button 4 is pressed, decrease the value but only if it's greater than 0
    if (button4State && !lastButton4State && !button5State) {
      if (value > 0) {  // Only decrease if value is greater than 0
        value = max(value - 1, 1);  // Decrease the value, ensuring it doesn't go below 1
      }
      lastDebounceTime = millis();
    }

    
    // If button 5 was just pressed and button 4 is not pressed
    // Only allow value to increase from 0 to 1 or higher
    if (button5State && !lastButton5State && !button4State) {
      // If value is 0, set it to 1, otherwise increase it
      if (value == 0) {
        value = 1;  // Only button 5 can increase from 0 to 1
      } else {
        value = min(value + 1, 6);  // Increase the value, ensuring it doesn't go above 6
      }
      lastDebounceTime = millis();
    }
    
    // If button 4 and button 5 are both pressed simultaneously
    // Transition from 1 to 0 only when button 4 is held and button 5 is pressed
    if (value == 1 && button4State && button5State) {
      value = 0;  // Set value to 0
      lastDebounceTime = millis();  // Reset debounce timer
    }
  }

  // Save the current state of the buttons for the next loop
  lastButton4State = button4State;
  lastButton5State = button5State;

  // Only update the LED when the value changes
  static int lastValue = -1;  // Stores the previous value
  
  if (value != lastValue) {  // Only update if the value changed
    if (lastValue >= 0 && lastValue < 7) {
      digitalWrite(ledPins[lastValue], LOW);  // Turn off previous LED
    }
    if (value >= 0 && value < 7) {
      digitalWrite(ledPins[value], HIGH);  // Turn on the new LED
    }
    lastValue = value;  // Update lastValue
  }

  // **Non-blocking ESP-NOW send every sendInterval**
  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();  // Update last send time

    // Set value to send
    myData.b = value;

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    if (result == ESP_OK) {
      Serial.print("Sent value: ");
      Serial.println(value);
    } else {
      Serial.print("Error sending the data, code: ");
      Serial.println(result);
    }
  }
}


