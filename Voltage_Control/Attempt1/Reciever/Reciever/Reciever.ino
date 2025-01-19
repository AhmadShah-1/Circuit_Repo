#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// RF24 Setup
RF24 radio(7, 8);  // CE, CSN pins
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  // Receive and print voltage data
  if (radio.available()) {
    char receivedData[32];
    radio.read(&receivedData, sizeof(receivedData));
    Serial.println(receivedData);
  }

  // Check for user input to send commands
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'p' || command == 'c') {
      radio.stopListening();
      radio.write(&command, sizeof(command));
      radio.startListening();
    }
  }

  delay(500);
}
