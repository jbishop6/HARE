#include <esp_now.h>
#include <WiFi.h>

// Change this to the MAC address of ESP32-A
uint8_t peerMACAddress[] = { 0xCC, 0x8D, 0xA2, 0x0F, 0xC9, 0x84 };  

typedef struct message_t {
    char text[32];
} message_t;

message_t myMessage;

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    message_t receivedMessage;
    memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));

    Serial.print("Received from ESP32-A: ");
    Serial.println(receivedMessage.text);

    // Optional: Send a response back
    strcpy(myMessage.text, "Bye");
    esp_now_send(peerMACAddress, (uint8_t *)&myMessage, sizeof(myMessage));
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Message send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }

    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, peerMACAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    // Send initial message
    strcpy(myMessage.text, "Bye");
    esp_now_send(peerMACAddress, (uint8_t *)&myMessage, sizeof(myMessage));
}

void loop() {
    delay(3000);  // Send message every 3 seconds
    esp_now_send(peerMACAddress, (uint8_t *)&myMessage, sizeof(myMessage));
}
