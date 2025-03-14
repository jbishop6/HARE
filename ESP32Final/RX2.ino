#include <esp_now.h>
#include <WiFi.h>
#include <string>
// Change this to the MAC address of ESP32-A
uint8_t peerMACAddress[] = { 0xCC, 0x8D, 0xA2, 0x0F, 0xC6, 0x38 };  

typedef struct message_t {
    char text[32];
} message_t;

message_t myMessage;
std::string success;

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    message_t receivedMessage;
    memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));

    Serial.print("Received from ESP32-A: ");
    Serial.println(receivedMessage.text);

    // Optional: Send a response back
    strcpy(myMessage.text, "Bye");
    esp_now_send(peerMACAddress, (uint8_t *)&myMessage, sizeof(myMessage));
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting ESP32...");

    WiFi.mode(WIFI_STA);  // REQUIRED for ESP-NOW
    Serial.println("WiFi mode set to STA");

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    } else {
        Serial.println("ESP-NOW initialized successfully!");
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(onDataRecv);
    Serial.println("Callbacks registered.");

    // Remove old peer (if exists) before adding a new one
    esp_now_del_peer(peerMACAddress);
    
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));  // Ensure struct is clean
    memcpy(peerInfo.peer_addr, peerMACAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer!");
        return;
    } else {
        Serial.println("Peer added successfully!");
    }

    // Send initial message
    strcpy(myMessage.text, "Hello!");
    Serial.println("Sending first message...");
    esp_err_t result = esp_now_send(peerMACAddress, (uint8_t *)&myMessage, sizeof(myMessage));

    if (result == ESP_OK) {
        Serial.println("Message sent successfully!");
    } else {
        Serial.println("Message sending failed!");
    }
}
   

void loop() {
    delay(3000);  // Send message every 3 seconds
    esp_now_send(peerMACAddress, (uint8_t *)&myMessage, sizeof(myMessage));
}
