#include <esp_now.h> 
#include <WiFi.h> 

#define buttonUpPin 12   // Change to the correct pin for ButtonUp 
#define buttonDownPin 13  // Change to the correct pin for ButtonDown 

// REPLACE WITH YOUR RECEIVER MAC Address 

uint8_t broadcastAddress[] = {0xcc, 0x8d, 0xa2, 0x0f, 0xc9, 0x84}; 

// Structure to send data (must match receiver structure) 

typedef struct struct_message { 

  bool UP; 

  bool DOWN; 

} struct_message; 

struct_message myData; 

esp_now_peer_info_t peerInfo; 

// Callback function when data is sent 

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) { 

  Serial.print("\r\nLast Packet Send Status:\t"); 

  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail"); 

} 

void setup() { 

  Serial.begin(115200); 

  // Set ESP32 to Wi-Fi Station mode 

  WiFi.mode(WIFI_STA); 

  // Set up the button pins as inputs with internal pull-down resistors 

  delay(500); 

  // Initialize ESP-NOW 

  if (esp_now_init() != ESP_OK) { 

    Serial.println("Error initializing ESP-NOW"); 

    return; 

  } 

  // Register send callback function 

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

  pinMode(buttonUpPin, INPUT_PULLDOWN); 

  pinMode(buttonDownPin, INPUT_PULLDOWN); 

} 

void loop() { 

  // Read actual button states 

  myData.UP = digitalRead(buttonUpPin); 

  myData.DOWN = digitalRead(buttonDownPin); 

  // Send message via ESP-NOW 

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); 

  if (result == ESP_OK) { 

    Serial.println("Sent with success"); 

  } else { 

    Serial.println("Error sending the data"); 

  } 

  delay(10);  // Increased delay to reduce potential collisions 

} 
