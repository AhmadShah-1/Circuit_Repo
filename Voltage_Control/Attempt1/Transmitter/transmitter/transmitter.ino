#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Digital Potentiometer Pins
#define CS 2
#define UD 3
#define INC 4

// Voltage Sensor Pin
#define VOLTAGE_PIN A0
const float rOhm1 = 30000.0; // Resistor 1 value in the voltage divider
const float rOhm2 = 7500.0;  // Resistor 2 value in the voltage divider
float refVoltage = 3.3;      // Reference voltage of the Arduino
int adcValue = 0;

// RF24 Setup
RF24 radio(7, 8);  // CE, CSN pins
const byte address[6] = "00001";

int currentStep = 0;   // Tracks the current step (0 to 99)
const int maxSteps = 99;

void setup() {
  Serial.begin(9600);
  pinMode(CS, OUTPUT);
  pinMode(UD, OUTPUT);
  pinMode(INC, OUTPUT);

  digitalWrite(CS, HIGH);
  digitalWrite(INC, HIGH);
  digitalWrite(UD, LOW);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  // Measure voltage
  adcValue = analogRead(VOLTAGE_PIN);
  float sensorOutVoltage = (adcValue * refVoltage) / 1024.0;
  float inputVoltage = sensorOutVoltage * (rOhm1 + rOhm2) / rOhm2;

  // Send the measured voltage
  char voltageData[32];
  snprintf(voltageData, sizeof(voltageData), "Voltage: %.2f V", inputVoltage);
  radio.stopListening();
  radio.write(&voltageData, sizeof(voltageData));
  radio.startListening();

  // Check for incoming commands
  if (radio.available()) {
    char command;
    radio.read(&command, sizeof(command));
    if (command == 'p' && currentStep > 0) {
      // Decrease resistance
      digitalWrite(CS, LOW);
      digitalWrite(UD, LOW);
      digitalWrite(INC, HIGH);
      delay(50);
      digitalWrite(INC, LOW);
      currentStep--;
    } else if (command == 'c' && currentStep < maxSteps) {
      // Increase resistance
      digitalWrite(CS, LOW);
      digitalWrite(UD, HIGH);
      digitalWrite(INC, HIGH);
      delay(50);
      digitalWrite(INC, LOW);
      currentStep++;
    }
  }

  delay(500);
}
