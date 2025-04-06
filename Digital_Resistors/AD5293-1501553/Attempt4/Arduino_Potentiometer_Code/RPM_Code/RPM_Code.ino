#include <SPI.h>

const int CS_PIN = 10; // Chip Select Pin for AS5048
uint16_t zeroOffset = 0; // Stores the reference angle

// Variables for Speed Calculation
uint16_t prevAngle = 0;
unsigned long prevTime = 0;

// Moving Average Filter
const int NUM_SAMPLES = 5; // Number of samples for smoothing
float rpmSamples[NUM_SAMPLES] = {0}; // Circular buffer for RPM
int sampleIndex = 0;

// Function to initialize SPI communication
void setup() {
    Serial.begin(9600);
    SPI.begin();
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));

    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH); // Ensure CS is high at the start

    delay(1000); // Allow sensor to stabilize
    setZeroReference(); // Set initial zero angle when the blade is in the reference position
}

// Function to read raw angle from AS5048
uint16_t readAS5048Angle() {
    uint16_t angle = 0;
    digitalWrite(CS_PIN, LOW); // Enable sensor

    // Read 14-bit angle value
    uint8_t highByte = SPI.transfer(0xFF);
    uint8_t lowByte = SPI.transfer(0xFF);

    digitalWrite(CS_PIN, HIGH); // Disable sensor

    angle = ((highByte & 0x3F) << 8) | lowByte; // Mask 14-bit value
    return angle;
}

// Function to set zero reference when the blade is at the desired starting position
void setZeroReference() {
    zeroOffset = readAS5048Angle(); // Store current angle as zero reference
}

// Function to get the corrected angle
float getCorrectedAngle() {
    uint16_t rawAngle = readAS5048Angle();
    int16_t correctedAngle = rawAngle - zeroOffset;

    if (correctedAngle < 0) correctedAngle += 16384; // Ensure positive angle

    return (correctedAngle * 360.0) / 16383.0; // Convert to degrees
}

// Function to apply moving average filter for RPM smoothing
float getSmoothedRPM(float newRPM) {
    rpmSamples[sampleIndex] = newRPM;
    sampleIndex = (sampleIndex + 1) % NUM_SAMPLES; // Circular buffer

    float sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += rpmSamples[i];
    }
    return sum / NUM_SAMPLES; // Return averaged RPM
}

void loop() {
    unsigned long currentTime = millis();
    float degrees = getCorrectedAngle(); // Use corrected angle

    // Speed Calculation
    int16_t angleDifference = (degrees * 16383.0) / 360.0 - prevAngle;
    if (angleDifference < -8192) angleDifference += 16384;
    else if (angleDifference > 8192) angleDifference -= 16384;

    float timeDifference = (currentTime - prevTime) / 1000.0;
    float speedDegreesPerSecond = (angleDifference * 360.0) / 16384.0 / timeDifference;
    float speedRPM = speedDegreesPerSecond * (1.0 / 360.0) * 60.0;

    // Apply moving average smoothing
    float smoothedRPM = getSmoothedRPM(speedRPM);

    // Send Data to Serial (Excel Data Streamer)
    Serial.print(currentTime);
    Serial.print(",");
    Serial.print(degrees);
    Serial.print(",");
    Serial.println(smoothedRPM);

    // Update previous values
    prevAngle = (degrees * 16383.0) / 360.0;
    prevTime = currentTime;
   
    delay(50); // Sampling every 50ms
}