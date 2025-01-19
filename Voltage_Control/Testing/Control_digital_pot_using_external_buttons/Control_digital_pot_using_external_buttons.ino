/* Digital Pot 2 Button UP and Down Volume by Peter Forrer
 *  Modified to include debouncing and INPUT_PULLUP for buttons.
 */

int pot = A0; // Analog pin to read the potentiometer wiper output (RW)
int b1 = 10;  // Button 1 pin (Increase resistance)
int b2 = 11;  // Button 2 pin (Decrease resistance)

int buttonState = 0;   // Current state of button 1
int buttonState2 = 0;  // Current state of button 2

unsigned long lastPressTimeB1 = 0; // Last press time for button 1
unsigned long lastPressTimeB2 = 0; // Last press time for button 2

const int debounceDelay = 50; // Debounce delay in milliseconds

#define CS 2    // Chip Select pin
#define UD 3    // Up/Down pin
#define INC 4   // Increment pin

void setup() {
  Serial.begin(9600);
  Serial.println("X9C104S Digital POT Test");

  // Digital Potentiometer pins
  pinMode(CS, OUTPUT);
  pinMode(UD, OUTPUT);
  pinMode(INC, OUTPUT);

  // Buttons with internal pull-up resistors
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);

  // Ensure the digital pot starts in a known state
  digitalWrite(CS, HIGH);
  digitalWrite(INC, HIGH);
  digitalWrite(UD, LOW);
}

//===================================================
// Function to store the current potentiometer setting
void store() {
  digitalWrite(INC, HIGH);
  digitalWrite(CS, HIGH);
  delay(50);
  digitalWrite(CS, LOW);
}

// Function to increase resistance
void Up() {
  digitalWrite(CS, LOW);
  digitalWrite(UD, HIGH);
  digitalWrite(INC, HIGH);
  delay(50);
  digitalWrite(INC, LOW);
}

// Function to decrease resistance
void Down() {
  digitalWrite(CS, LOW);
  digitalWrite(UD, LOW);
  digitalWrite(INC, HIGH);
  delay(50);
  digitalWrite(INC, LOW);
}
//======================================================

void loop() {
  // Read button states
  buttonState = digitalRead(b1);
  buttonState2 = digitalRead(b2);

  // Handle button 1 (Increase resistance)
  if (buttonState == LOW && millis() - lastPressTimeB1 > debounceDelay) {
    lastPressTimeB1 = millis(); // Update last press time
    Up();
  }

  // Handle button 2 (Decrease resistance)
  if (buttonState2 == LOW && millis() - lastPressTimeB2 > debounceDelay) {
    lastPressTimeB2 = millis(); // Update last press time
    Down();
  }

  // Store the current setting if both buttons are pressed
  if (buttonState == LOW && buttonState2 == LOW) {
    store();
  }

  // Read and print the analog value from the wiper (RW pin)
  int Read = analogRead(pot);
  Serial.print("Analog reading = ");
  Serial.println(Read);

  // Print button states for debugging
  Serial.print("Button 1 state: ");
  Serial.println(buttonState);
  Serial.print("Button 2 state: ");
  Serial.println(buttonState2);

  delay(500); // Small delay to stabilize readings
}
