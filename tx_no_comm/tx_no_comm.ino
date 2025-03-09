// Define button pins
const int buttonPin4 = 4;
const int buttonPin5 = 5;

// Define LED pins (corresponding to values 0-6)
const int ledPins[7] = {18, 8, 3, 46, 9, 10, 11};

// Variable to hold the value
int value = 0;

// Variables to track the button states
bool lastButton4State = LOW;  // Previous state of button 4
bool lastButton5State = LOW;  // Previous state of button 5
unsigned long lastDebounceTime = 0;  // Last time the button state changed
unsigned long debounceDelay = 50;    // Delay for debouncing

void setup() {
  // Start serial communication for monitoring
  Serial.begin(115200);

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
    
    // Transition from 1 to 0 only when button 4 is held and button 5 is pressed simultaneously
    if (value == 1 && button4State && button5State) {
      value = 0;  // Set value to 0
      lastDebounceTime = millis();  // Reset debounce timer
    }
    
    // If button 4 was just pressed and button 5 is not pressed, decrease the value
    if (button4State && !lastButton4State && !button5State) {
      value = max(value - 1, 1);  // Decrease the value, ensuring it doesn't go below 1
      lastDebounceTime = millis();  // Reset debounce timer
    }
    
    // If button 5 was just pressed and button 4 is not pressed, increase the value
    if (button5State && !lastButton5State && !button4State) {
      value = min(value + 1, 6);  // Increase the value, ensuring it doesn't go above 6
      lastDebounceTime = millis();  // Reset debounce timer
    }
  }

  // Save the current state of the buttons for the next loop
  lastButton4State = button4State;
  lastButton5State = button5State;

  // Output the value to the serial monitor
  Serial.println(value);

  // Turn off all LEDs
  for (int i = 0; i < 7; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  // Turn on the LED corresponding to the current value
  if (value >= 0 && value < 7) {
    digitalWrite(ledPins[value], HIGH);  // Turn on the corresponding LED
  }
}
