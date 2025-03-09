#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
typedef struct struct_message {
    int b;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function when data is received
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.println();
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

  // Register the callback function to handle received data
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Empty loop because data is received via callback
}

