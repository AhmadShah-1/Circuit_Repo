#include <SPI.h>

// Command definitions
#define POT_CMD_NOP         0x00u  // No Operation
#define POT_CMD_WRITE       0x01u  // Write to RDAC register
#define POT_CMD_READ        0x02u  // Read from RDAC register
#define POT_CMD_SW_RESET    0x03u  // Software reset command
#define POT_CMD_WR_CTRL     0x06u  // Write to Control Register (working command)
#define POT_CMD_RD_CTRL     0x05u  // Read from Control Register

// Control bit to allow wiper writes (bit C1; 0x1 << 1 = 2)
#define POT_CTRL_WIPER_ALLOW_WRITE  (0x1 << 1)

// Bit positions and masks
#define POT_CMD_Pos         10     // Command is in bits [15:10]
#define POT_DATA_Msk        0x03FFu  // Data mask (10 bits)

// Helper function to build a 16-bit command word.
uint16_t combine(uint8_t cmd, uint16_t data) {
  return ((uint16_t)cmd << POT_CMD_Pos) | (data & POT_DATA_Msk);
}

// Pin configuration:
const uint8_t pinCS    = 8;   // Connect to AD5293 SYNC (!CS)
const uint8_t pinRDY   = 9;   // Connect to AD5293 RDY (optional)
const uint8_t pinRESET = 10;  // Connect to AD5293 RESET

// Reset routines
void hardwareReset() {
  Serial.println("Performing hardware reset...");
  digitalWrite(pinRESET, LOW);
  delay(10);
  digitalWrite(pinRESET, HIGH);
  delay(10);
  Serial.println("Hardware reset complete.");
}

void softwareReset() {
  Serial.println("Performing software reset (Command 3)...");
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  digitalWrite(pinCS, LOW);
  SPI.transfer16(combine(POT_CMD_SW_RESET, 0x000));
  digitalWrite(pinCS, HIGH);
  SPI.endTransaction();
  delay(10);
  Serial.println("Software reset complete.");
}

// Write a value to the RDAC register.
void writeWiper(uint16_t value) {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  digitalWrite(pinCS, LOW);
  SPI.transfer16(combine(POT_CMD_WRITE, value));
  digitalWrite(pinCS, HIGH);
  SPI.endTransaction();
}

// Write to the control register.
void writeControlRegister(uint16_t value) {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  digitalWrite(pinCS, LOW);
  SPI.transfer16(combine(POT_CMD_WR_CTRL, value));
  digitalWrite(pinCS, HIGH);
  SPI.endTransaction();
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor
  
  // Initialize SPI and control pins.
  SPI.begin();
  pinMode(pinCS, OUTPUT);
  digitalWrite(pinCS, HIGH);
  pinMode(pinRESET, OUTPUT);
  digitalWrite(pinRESET, HIGH);
  pinMode(pinRDY, INPUT_PULLUP);
  
  Serial.println("Starting AD5293 SPI control demo");
  Serial.print("Initial RDY pin state: ");
  Serial.println(digitalRead(pinRDY));
  
  // Initialize the chip.
  hardwareReset();
  softwareReset();
  Serial.println("Enabling wiper writes...");
  writeControlRegister(POT_CTRL_WIPER_ALLOW_WRITE);
  delay(10);
  
  // Set an initial wiper value (for example, 512).
  writeWiper(512);
  Serial.println("AD5293 initialized.");
  Serial.println("Waiting for new wiper step from Raspberry Pi (0-1023):");
}

void loop() {
  // Use readStringUntil to get the full line and avoid stray newline characters.
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove whitespace/newlines
    if (input.length() > 0) {
      int step = input.toInt();
      step = constrain(step, 0, 1023);
      writeWiper(step);
      Serial.print("Wiper set to: ");
      Serial.println(step);
    }
  }
}
