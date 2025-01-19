/**
 * Receiver that displays incoming data and sets an
 * ack payload containing the user command ('p' or 'c').
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Radio wiring: CE=7, CSN=8
RF24 radio(7, 8);
const byte address[6] = "00001";

// We'll store the last user command from Serial
// so that we can send it in the next ack payload.
char userCommand = '\0';

void setup() {
  Serial.begin(9600);
  Serial.println("Receiver with AckPayload Starting...");

  if (!radio.begin()) {
    Serial.println("nRF24L01 not detected. Check connections!");
    while (true);
  }
  radio.setPALevel(RF24_PA_MIN);

  // Ack payload
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.enableAckPayload();

  // Listen on the same address
  radio.openReadingPipe(1, address);
  radio.startListening();

  Serial.println("Receiver setup complete!");
}

void loop() {
  // 1) Check if we got data from the transmitter
  if (radio.available()) {
    // We'll read the dynamic payload
    char incoming[32] = {0};
    byte len = radio.getDynamicPayloadSize(); // how many bytes actually arrived
    if (len > 0 && len <= 32) {
      radio.read(&incoming, len);
      // Null-terminate just to be safe
      incoming[len] = '\0';

      // Print it
      Serial.print("Received from transmitter: ");
      Serial.println(incoming);

      // 2) Send an Ack Payload if we have a user command waiting
      if (userCommand != '\0') {
        // Build a short ack payload
        char ackPayload[2];
        ackPayload[0] = userCommand;
        ackPayload[1] = '\0';

        // Send that ack payload automatically on pipe #1
        radio.writeAckPayload(1, ackPayload, strlen(ackPayload) + 1);
        Serial.print("Ack Payload set to: ");
        Serial.println(ackPayload);

        // If you want to send the command only once, clear it
        userCommand = '\0';
      } else {
        // Otherwise you can send nothing or an empty ack
        // radio.writeAckPayload(1, "", 1); 
      }
    }
  }

  // 3) Check for user input from Serial
  //    'p' => Decrease pot, 'c' => Increase pot
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "p") {
      userCommand = 'p';
      Serial.println("User command queued: p");
    } else if (cmd == "c") {
      userCommand = 'c';
      Serial.println("User command queued: c");
    } else {
      Serial.println("Invalid command. Use 'p' or 'c'.");
    }
  }
}
