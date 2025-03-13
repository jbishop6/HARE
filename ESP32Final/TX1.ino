#include <esp_now.h>
#include <WiFi.h>

// Structure for ESP-NOW message
typedef struct Message {
    char command; // 'U' = Upshift, 'D' = Downshift
} Message;

Message shiftMessage;

// ✅ Updated RX2 MAC Address
uint8_t rx2Address[] = {0xCC, 0x8D, 0xA2, 0x0F, 0xC9, 0x84}; 

// Button Pins
const int downshiftButton = 4;
const int upshiftButton = 5;

// LED Pins (Gears 0 - 6)
const int gearLeds[] = {18, 8, 3, 46, 9, 10, 11};

// Gear Tracking
int currentGear = 0; // Starts in Neutral (0)

// ESP-NOW Callback (Send Status)
void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    Serial.print("ESP-NOW Message Sent: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Update LED display
void updateGearIndicator() {
    for (int i = 0; i < 7; i++) {
        digitalWrite(gearLeds[i], LOW);
    }
    digitalWrite(gearLeds[currentGear], HIGH); // Light up current gear LED
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_register_send_cb(onSent);
    
    // Add RX2 as a peer
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, rx2Address, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    // Button Setup
    pinMode(downshiftButton, INPUT_PULLUP);
    pinMode(upshiftButton, INPUT_PULLUP);

    // LED Setup
    for (int i = 0; i < 7; i++) {
        pinMode(gearLeds[i], OUTPUT);
        digitalWrite(gearLeds[i], LOW); // Turn off all LEDs
    }

    updateGearIndicator();
}

void loop() {
    if (digitalRead(upshiftButton) == LOW) { 
        if (currentGear < 6) {
            currentGear++;
            shiftMessage.command = 'U';
            esp_now_send(rx2Address, (uint8_t *)&shiftMessage, sizeof(shiftMessage));
            Serial.println("Sent: Upshift");
            updateGearIndicator();
            delay(300);
        }
    }

    if (digitalRead(downshiftButton) == LOW) { 
        if (currentGear > 0) {
            currentGear--;
            shiftMessage.command = 'D';
            esp_now_send(rx2Address, (uint8_t *)&shiftMessage, sizeof(shiftMessage));
            Serial.println("Sent: Downshift");
            updateGearIndicator();
            delay(300);
        }
    }
}
