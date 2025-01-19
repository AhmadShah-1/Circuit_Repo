/* Digital Pot Automatic Step Control */

#define CS 2    // Chip Select pin
#define UD 3    // Up/Down pin
#define INC 4   // Increment pin

const int voltagePin = A0; // Voltage module connected to A0
int currentStep = 0;       // Tracks the current step (0 to 99)
const int maxSteps = 99;   // Maximum number of steps for the digital pot
unsigned long startTime = 0; // Time when the Arduino started
bool initialized = false;  // Flag to indicate if initialization is complete

// Voltage divider resistors
const float rOhm1 = 30000.0; // Resistor 1 Value (30 kΩ)
const float rOhm2 = 7500.0;  // Resistor 2 Value (7.5 kΩ)

// Arduino base voltage (adjust to match your actual measured voltage)
float baseVoltage = 5.12; 

void setup() {
  Serial.begin(9600);
  Serial.println("X9C104S Digital POT Test");

  // Digital Potentiometer pins
  pinMode(CS, OUTPUT);
  pinMode(UD, OUTPUT);
  pinMode(INC, OUTPUT);

  // Ensure the digital pot starts in a known state
  digitalWrite(CS, HIGH);
  digitalWrite(INC, HIGH);
  digitalWrite(UD, LOW);

  startTime = millis();
}

// Function to decrease resistance
void Down() {
  digitalWrite(CS, LOW);
  digitalWrite(UD, LOW); // Set to decrease resistance
  digitalWrite(INC, HIGH);
  delay(50);
  digitalWrite(INC, LOW);
  currentStep--;
}

// Function to increase resistance
void Up() {
  if (currentStep < maxSteps) {
    digitalWrite(CS, LOW);
    digitalWrite(UD, HIGH); // Set to increase resistance
    digitalWrite(INC, HIGH);
    delay(50);
    digitalWrite(INC, LOW);
    currentStep++;
  }
}

void loop() {
  // Ensure potentiometer is at the lowest value during the first 5 seconds
  if (!initialized && millis() - startTime < 5000) {
    Down();
    delay(100); // Short delay to ensure stability during initialization
    return;
  } else if (!initialized) {
    initialized = true;
    Serial.println("Initialization complete. Ready for step control.");
  }

  // Read user input from the serial monitor
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command == 'c') {
      Up(); // Move to the next step
    } else if (command == 'p') {
      Down(); // Move to the previous step
    }
  }

  // Read the voltage on the pin
  int a1PinVolts = analogRead(voltagePin);
  float sensorOutVolts = (a1PinVolts * baseVoltage) / 1024.0;

  // Calculate the input voltage to the divider
  float inputVoltage = sensorOutVolts * (rOhm1 + rOhm2) / rOhm2;

  // Print voltage information
  Serial.print("Measured Voltage at Pin: ");
  Serial.print(sensorOutVolts);
  Serial.println(" V");

  Serial.print("Calculated Input Voltage: ");
  Serial.print(inputVoltage);
  Serial.println(" V");

  // Display current step
  Serial.print("Current Step: ");
  Serial.println(currentStep);

  delay(500); // Small delay to stabilize readings
}
