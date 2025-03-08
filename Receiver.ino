#include <esp_now.h>
#include <WiFi.h>

#define BLUE_LED 12  // Blue LED Output
#define RED_LED 13   // Red LED Output

typedef struct struct_message {
    int command;
} struct_message;

struct_message receivedMsg;

// 🔥 Updated Callback Function (ESP-IDF v5 Compatible)
void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    memcpy(&receivedMsg, incomingData, sizeof(receivedMsg));

    if (receivedMsg.command == 1) {
        digitalWrite(BLUE_LED, HIGH);
        digitalWrite(RED_LED, LOW);
        Serial.println("Blue LED ON");
    }
    else if (receivedMsg.command == 2) {
        digitalWrite(BLUE_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        Serial.println("Red LED ON");
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);  // Set ESP32 as a station

    pinMode(BLUE_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed!");
        return;
    }

    // 🔥 Update Callback Registration (ESP-IDF v5)
    esp_now_register_recv_cb(onReceive);
}

void loop() {
    // Nothing needed here; ESP-NOW works in the background
}
