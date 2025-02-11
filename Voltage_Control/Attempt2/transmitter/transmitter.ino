#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Radio wiring: CE = 7, CSN = 8
RF24 radio(7, 8);
const byte address[6] = "00001"; // Both nodes must use the same address

void setup() {
  Serial.begin(115200);
  Serial.println("Transmitter starting...");
  delay(500);

  if (!radio.begin()) {
    Serial.println("nRF24L01 not detected. Check connections!");
    while (true);
  }
  radio.setPALevel(RF24_PA_LOW);
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.setRetries(5, 15); // Optional: adjust retry settings

  // Open pipes for writing and reading (for ACKs)
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);

  // Put the radio in TX mode
  radio.stopListening();
}

void loop() {
  const char payload[] = "dog";
  
  // Send "dog" (include the terminating '\0')
  bool ok = radio.write(&payload, strlen(payload) + 1);
  Serial.print("Sent: ");
  Serial.println(payload);

  if (ok) {
    // Check if an ACK payload is available from the receiver
    if (radio.isAckPayloadAvailable()) {
      uint8_t ackSize = radio.getDynamicPayloadSize();
      char ackBuffer[32] = {0};
      radio.read(&ackBuffer, ackSize);
      Serial.print("Ack received: ");
      Serial.println(ackBuffer);
    } else {
      Serial.println("No ACK payload received.");
    }
  } else {
    Serial.println("Transmission failed.");
  }
  
  delay(100); // Wait a second before sending again
}
