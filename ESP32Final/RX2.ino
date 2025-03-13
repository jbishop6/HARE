#include <esp_now.h>
#include <WiFi.h>

// Structure to receive ESP-NOW messages
typedef struct Message {
    char command; // 'U' = Upshift, 'D' = Downshift
} Message;

Message receivedMessage;

// LED & Relay Pins
const int downshiftIndicator = 4; // Red LED + Downshift Relay
const int upshiftIndicator = 5;   // Yellow LED + Upshift Relay

// ESP-NOW Callback (Handles Incoming Data)
void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));
    Serial.print("Received Command: ");
    Serial.println(receivedMessage.command);

    if (receivedMessage.command == 'U') {
        activateShift(upshiftIndicator, 250); // Upshift: Yellow LED & relay
    } else if (receivedMessage.command == 'D') {
        activateShift(downshiftIndicator, 350); // Downshift: Red LED & relay
    }
}

// Activate relay & LED for a shift
void activateShift(int pin, int duration) {
    Serial.print("Activating relay & LED on Pin ");
    Serial.print(pin);
    Serial.print(" for ");
    Serial.print(duration);
    Serial.println(" ms");

    digitalWrite(pin, HIGH); // Turn ON LED & relay
    delay(duration);
    digitalWrite(pin, LOW);  // Turn OFF LED & relay
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_register_recv_cb(onReceive);

    // Set LED & relay pins as OUTPUT
    pinMode(downshiftIndicator, OUTPUT);
    pinMode(upshiftIndicator, OUTPUT);

    digitalWrite(downshiftIndicator, LOW); // Start with relays OFF
    digitalWrite(upshiftIndicator, LOW);
}

void loop() {
    // Nothing needed here; ESP-NOW receives automatically
}
