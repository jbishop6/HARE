#include <esp_now.h> 
#include <WiFi.h> 
#define LED_UP 4 // Yellow LED 
#define LED_DOWN 5 // RED LED 

const int switchPins[7] = {37, 36, 35, 41, 39, 40, 38}; 
int TimeIntoN = 75;  // For gear 0, when shifting into N 50-150 100-75 
int TimeOutofN = 75; // For gear 1, when shifting into 1 50-100 100-75 
int TimeUp = 125;     // For gears 1-5, when shifting up 50-250 100-125 
int TimeDown = 125;   // For gears 2-6, when shifting down 50-150 100-125 
int AfterShift = 50; // delay after shift 50-100 100-50 

unsigned long startTime = 0; 
bool timerActive = false; 
bool UP_OLD = false; 
bool DOWN_OLD = false; 
int currentGear = -1;  // Always start in gear 0 

//int newGear; 
// Structure to receive data (must match sender structure) 

typedef struct struct_message { 
  bool UP; 
  bool DOWN; 
} struct_message; 

struct_message myData; 

void setup() { 
  Serial.begin(115200); 
  // Set ESP32 to Wi-Fi Station mode 
  WiFi.mode(WIFI_STA); 
  delay(500); 
  // Initialize ESP-NOW 
  if (esp_now_init() != ESP_OK) { 
    Serial.println("Error initializing ESP-NOW"); 
    return; 
  } 

  delay(500); 
  // Register receive callback function (fixed for new ESP-NOW version) 

  esp_now_register_recv_cb(OnDataRecv); 
  delay(500); 

  // Only update the LED when the value changes 

  pinMode(LED_UP, OUTPUT);  // Set LED pins as outputs 
  pinMode(LED_DOWN, OUTPUT); 
  for (int i = 0; i < 7; i++) { 
    pinMode(switchPins[i], INPUT_PULLUP); 
  } 
} 

// Updated callback function 
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) { 
  memcpy(&myData, incomingData, sizeof(myData)); 
  Serial.print("Bytes received: "); 
  Serial.println(len); 
  Serial.print("Bool UP: "); 
  Serial.println(myData.UP); 
  Serial.print("Bool DOWN: "); 
  Serial.println(myData.DOWN); 
  Serial.print("Gear Position: "); 
  Serial.println(currentGear); 
  Serial.println(); 

  for (int i = 0; i < 7; i++) { 
    if (digitalRead(switchPins[i]) == LOW) { // LOW means button is pressed 
      currentGear = i; 
    } 
  } 
  if (currentGear == 0) { 
    if (myData.UP == true && UP_OLD == false) { 
      digitalWrite(LED_DOWN, HIGH); 
      delay(TimeOutofN); 
      digitalWrite(LED_DOWN, LOW); 
      delay(AfterShift); 
      //newGear++; 
    } 
  } 
if (currentGear >= 1 && currentGear <= 5) { 
  if (myData.UP == true && UP_OLD == false) { 
      digitalWrite(LED_UP, HIGH);  // Turn LED ON   
      delay(TimeUp);  // Wait for the delay time 
      digitalWrite(LED_UP, LOW); 
      delay(AfterShift); 
      //newGear++; 
    } 
  } 
  if (currentGear >= 2 && currentGear <= 6) { 
    if (myData.DOWN == true && DOWN_OLD == false) { 
      digitalWrite(LED_DOWN, HIGH);  // Turn LED ON   
      delay(TimeDown);  // Wait for the delay time 
      digitalWrite(LED_DOWN, LOW);  // Turn LED ON   
      delay(AfterShift); 
      //newGear--; 
    } 
  } 
  if (currentGear == 1) { 
    if (myData.DOWN == true) { 
      if (!timerActive) { 
        startTime = millis();  // Start the timer 
        timerActive = true; 
      } 
      if (millis() - startTime >= 2000) { 
        digitalWrite(LED_UP, HIGH);  // Turn LED ON 
        delay(TimeIntoN); 
        digitalWrite(LED_UP, LOW); 
        delay(AfterShift); 
        //newGear--; 
      } 
    } 
    else { 
        timerActive = false;  // Reset timer 
      } 
  } 
  UP_OLD = myData.UP; 
  DOWN_OLD = myData.DOWN; 
} 

void loop() { 
  // The loop can remain empty as the callback handles received data 
} 
