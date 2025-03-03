import serial
import time

# Update the serial port if necessary (e.g., '/dev/ttyACM0' or '/dev/ttyUSB0')
SERIAL_PORT = '/dev/ttyACM0'
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
except serial.SerialException as e:
    print("Error opening serial port:", e)
    exit(1)

# Allow time for the Arduino to reset
time.sleep(2)
print("Connected to Arduino on", SERIAL_PORT)

try:
    while True:
        user_input = input("Enter new wiper step (0-1023): ").strip()
        if not user_input:
            continue
        try:
            step = int(user_input)
        except ValueError:
            print("Invalid input. Please enter an integer between 0 and 1023.")
            continue

        if step < 0 or step > 1023:
            print("Step out of range. Please enter a value between 0 and 1023.")
            continue

        # Calculate expected voltage output based on a 3.8V input.
        voltage_out = (step / 1023.0) * 3.8
        print("Calculated voltage output: {:.3f} V".format(voltage_out))
        
        # Send the step value to the Arduino (append newline).
        ser.write((str(step) + "\n").encode('utf-8'))
        
        # Optionally, wait a moment and print any responses from the Arduino.
        time.sleep(0.5)
        while ser.in_waiting:
            response = ser.readline().decode('utf-8', errors='replace').strip()
            if response:
                print("Arduino:", response)

except KeyboardInterrupt:
    print("\nExiting.")

finally:
    ser.close()
