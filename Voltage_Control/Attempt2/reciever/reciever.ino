#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Radio wiring: CE = 7, CSN = 8
RF24 radio(7, 8);
const byte address[6] = "00001"; // Must match the transmitter

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver starting...");

  if (!radio.begin()) {
    Serial.println("nRF24L01 not detected. Check connections!");
    while (true);
  }
  radio.setPALevel(RF24_PA_LOW);
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.setRetries(5, 15); // Optional: adjust retry settings
  Serial.print("start ");

  // Open the reading pipe and start listening
  radio.openReadingPipe(1, address);
  radio.startListening();
}

void loop() {
  // If a packet is available, read it...
  if (radio.available()) {
    char incoming[32] = {0};
    uint8_t len = radio.getDynamicPayloadSize();
    if (len > 0 && len < sizeof(incoming)) {
      radio.read(&incoming, len);
      incoming[len] = '\0';  // Ensure string termination
      Serial.print("Received: ");
      Serial.println(incoming);
    }
    
    // Set the ACK payload to "cat"
    const char ack[] = "cat";
    radio.writeAckPayload(1, ack, strlen(ack) + 1);
    Serial.print("ACK payload set to: ");
    Serial.println(ack);
  }
}
