/**
 * Transmitter that sends sensor data and processes
 * 'p'/'c' commands returned via Ack Payload
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Radio wiring: CE pin=7, CSN pin=8 (adjust to your setup)
RF24 radio(7, 8);

// Address must match the receiver
const byte address[6] = "00001";

// Digital Potentiometer Pins
#define CS  2
#define UD  3
#define INC 4

// Voltage Sensor Pin
#define VOLTAGE_PIN A0
const float rOhm1 = 30000.0;
const float rOhm2 = 7500.0;
const float refVoltage = 5;

int currentStep = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Transmitter with AckPayload Starting...");

  // Set up digital pot pins
  pinMode(CS, OUTPUT);
  pinMode(UD, OUTPUT);
  pinMode(INC, OUTPUT);
  digitalWrite(CS, HIGH);
  digitalWrite(INC, HIGH);
  digitalWrite(UD, LOW);

  // nRF24L01 initialization
  if (!radio.begin()) {
    Serial.println("nRF24L01 not detected. Check connections!");
    while (true); // Halt
  }
  radio.setPALevel(RF24_PA_MIN);

  // Enable dynamic payload and ack-payload features
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.enableAckPayload();

  // Use the same address for writing in this simple example
  radio.openWritingPipe(address);
  // You can open reading pipe #1 on the same address
  radio.openReadingPipe(1, address);

  Serial.println("Transmitter setup complete!");
}

void loop() {
  // 1) Read sensor (voltage)
  int adcValue = analogRead(VOLTAGE_PIN);
  float sensorOutVoltage = (adcValue * refVoltage) / 1024.0;
  float inputVoltage     = sensorOutVoltage * (rOhm1 + rOhm2) / rOhm2;

  // 2) Build a short string (must be <= 32 bytes total for nRF24!)
  // e.g., "3.30,Step=10"
  String dataString = String(inputVoltage, 2) + ",S=" + String(currentStep);
  char payload[32];
  dataString.toCharArray(payload, sizeof(payload));

  // 3) Send to receiver
  bool success = radio.write(payload, strlen(payload) + 1);
  if (!success) {
    Serial.println("Failed to send data");
  }

  // 4) Check if there's an Ack Payload waiting
  if (success && radio.isAckPayloadAvailable()) {
    char ackBuffer[8] = {0}; // Enough space for a short command + '\0'
    radio.read(&ackBuffer, radio.getDynamicPayloadSize());

    Serial.print("Received Ack Payload: ");
    Serial.println(ackBuffer);

    // If the receiver set 'p' or 'c', move the digital pot
    if (ackBuffer[0] == 'p') {
      // Decrease digital pot
      digitalWrite(CS, LOW);
      digitalWrite(UD, LOW);
      digitalWrite(INC, HIGH);
      delay(50);
      digitalWrite(INC, LOW);
      currentStep--;
      Serial.println("Decreased resistance.");
    }
    else if (ackBuffer[0] == 'c') {
      // Increase digital pot
      digitalWrite(CS, LOW);
      digitalWrite(UD, HIGH);
      digitalWrite(INC, HIGH);
      delay(50);
      digitalWrite(INC, LOW);
      currentStep++;
      Serial.println("Increased resistance.");
    }
  }

  delay(100); // Send data periodically
}
