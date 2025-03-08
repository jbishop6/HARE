#include <esp_now.h>
#include <WiFi.h>

#define BUTTON_BLUE 4   // Button to trigger BLUE LED
#define BUTTON_RED 5    // Button to trigger RED LED

// Replace with your Receiver ESP32's MAC Address
uint8_t receiverMAC[] = {0xCC, 0x8D, 0xA2, 0x0F, 0xC9, 0x84};


typedef struct struct_message {
    int command;
} struct_message;

struct_message msg;

void sendCallback(const uint8_t *mac, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message Sent!" : "Send Failed!");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    pinMode(BUTTON_BLUE, INPUT_PULLUP);
    pinMode(BUTTON_RED, INPUT_PULLUP);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed!");
        return;
    }

    esp_now_register_send_cb(sendCallback);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer!");
        return;
    }
}

void loop() {
    if (digitalRead(BUTTON_BLUE) == LOW) {
        msg.command = 1; // Command for Blue LED ON
        esp_now_send(receiverMAC, (uint8_t *)&msg, sizeof(msg));
        Serial.println("Sent: BLUE LED ON");
        delay(300);
    }

    if (digitalRead(BUTTON_RED) == LOW) {
        msg.command = 2; // Command for Red LED ON
        esp_now_send(receiverMAC, (uint8_t *)&msg, sizeof(msg));
        Serial.println("Sent: RED LED ON");
        delay(300);
    }
}

