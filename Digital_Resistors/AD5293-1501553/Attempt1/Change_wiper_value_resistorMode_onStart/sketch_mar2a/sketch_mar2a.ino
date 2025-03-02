#include <SPI.h>

// Command definitions (using working commands)
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

// Function to perform a hardware reset.
// A low-to-high transition on RESET loads midscale.
void hardwareReset() {
  Serial.println("Performing hardware reset...");
  digitalWrite(pinRESET, LOW);
  delay(10);
  digitalWrite(pinRESET, HIGH);
  delay(10);
  Serial.println("Hardware reset complete.");
}

// Function to perform a software reset via SPI.
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

// Read back the current wiper setting from the AD5293.
uint16_t readWiper() {
  uint16_t resp;
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  digitalWrite(pinCS, LOW);
  resp = SPI.transfer16(combine(POT_CMD_READ, 0x000));
  digitalWrite(pinCS, HIGH);
  delay(1);
  digitalWrite(pinCS, LOW);
  resp = SPI.transfer16(combine(POT_CMD_NOP, 0x000));
  digitalWrite(pinCS, HIGH);
  SPI.endTransaction();
  return (resp & POT_DATA_Msk);
}

// Read back the control register from the AD5293.
uint16_t readControlRegister() {
  uint16_t resp;
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  digitalWrite(pinCS, LOW);
  resp = SPI.transfer16(combine(POT_CMD_RD_CTRL, 0x000));
  digitalWrite(pinCS, HIGH);
  delay(1);
  digitalWrite(pinCS, LOW);
  resp = SPI.transfer16(combine(POT_CMD_NOP, 0x000));
  digitalWrite(pinCS, HIGH);
  SPI.endTransaction();
  return (resp & POT_DATA_Msk);
}

// Write a given value to the RDAC register.
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
  
  // Perform hardware and software resets.
  hardwareReset();
  softwareReset();
  
  // Enable wiper writes by clearing write-protect.
  Serial.println("Enabling wiper writes...");
  writeControlRegister(POT_CTRL_WIPER_ALLOW_WRITE);
  delay(10);
  
  uint16_t ctrl = readControlRegister();
  Serial.print("Control Register after write: 0x");
  Serial.println(ctrl, HEX);
  
  // Set initial wiper value to 512.
  Serial.println("Setting initial wiper value to 1...");
  writeWiper(1);
  delay(10);
  uint16_t wiperVal = readWiper();
  Serial.print("Read back wiper value: ");
  Serial.println(wiperVal);

  delay(5000);

  Serial.println("Setting initial wiper value to 300...");
  writeWiper(300);
  delay(10);
  wiperVal = readWiper();
  Serial.print("Read back wiper value: ");
  Serial.println(wiperVal);
  
  delay(5000);

  Serial.println("Setting initial wiper value to 1000...");
  writeWiper(1000);
  delay(10);
  wiperVal = readWiper();
  Serial.print("Read back wiper value: ");
  Serial.println(wiperVal);
  
  Serial.println("Enter a new wiper value (0-1023):");
}

void loop() {

}
