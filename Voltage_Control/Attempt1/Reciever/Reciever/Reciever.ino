#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// RF24 Setup
RF24 radio(7, 8);  // CE, CSN pins
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  Serial.println("Receiver Starting...");

  if (!radio.begin()) {
    Serial.println("nRF24L01 module not detected. Check connections!");
    while (true); // Halt execution
  }
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  Serial.println("nRF24L01 initialized successfully!");
}

void loop() {
  // Receive and print voltage data
  if (radio.available()) {
    char receivedData[32];
    radio.read(&receivedData, sizeof(receivedData));
    Serial.print("Received: ");
    Serial.println(receivedData);
  } else {
    Serial.println("No data available. Check transmitter.");
  }

  // Check for user input to send commands
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'p' || command == 'c') {
      radio.stopListening();
      if (radio.write(&command, sizeof(command))) {
        Serial.println("Command sent successfully!");
      } else {
        Serial.println("Failed to send command. Check module.");
      }
      radio.startListening();
    }
  }

}
