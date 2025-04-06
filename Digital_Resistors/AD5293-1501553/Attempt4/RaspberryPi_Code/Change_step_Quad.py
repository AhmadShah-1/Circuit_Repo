# Total duration for the sweep (seconds)
T_total = 10.0
voltage = 5.0



import serial
import time
import pandas as pd
import re

# Serial port settings
SERIAL_PORT = '/dev/ttyACM0'  # Update if needed
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
except serial.SerialException as e:
    print("Error opening serial port:", e)
    exit(1)

# Allow time for Arduino to reset
time.sleep(2)
print("Connected to Arduino on", SERIAL_PORT)



# Data storage for logging
data_records = []
start_time = time.time()
last_step = -1

print("Starting quadratic sweep from step 0 to 1023...")

while True:
    t = time.time() - start_time
    if t > T_total:
        break
    # Compute step quadratically: step = 1023 * (t/T_total)^2
    step = int(1023 * (t / T_total) ** 2)
    if step > 1023:
        step = 1023
    # Only update if the step has changed
    if step != last_step:
        voltage_out = (step / 1023.0) * voltage  # expected voltage output
        elapsed_time = t
        # Send step to Arduino
        ser.write((str(step) + "\n").encode('utf-8'))
        # Allow a short delay for Arduino to respond
        time.sleep(0.2)
        current_reading = None
        # Read available lines from Arduino
        while ser.in_waiting:
            line = ser.readline().decode('utf-8', errors='replace').strip()
            # Expected format: "Wiper set to: <step>, Current sensor reading: <value>"
            m = re.search(r"Wiper set to:\s*(\d+),\s*Current sensor reading:\s*(\d+)", line)
            if m:
                # Parse the current sensor value
                step_read = int(m.group(1))
                current_reading = int(m.group(2))
                print("Arduino:", line)
            else:
                print("Arduino:", line)
        if current_reading is None:
            current_reading = 0  # default if not received
        data_records.append({
            'Time (s)': elapsed_time,
            'Step': step,
            'Voltage (V)': voltage_out,
            'Current Sensor (raw)': current_reading
        })
        last_step = step
    time.sleep(0.05)

# Ensure final value is sent
ser.write((str(1023) + "\n").encode('utf-8'))
data_records.append({
    'Time (s)': time.time() - start_time,
    'Step': 1023,
    'Voltage (V)': voltage_out,
    'Current Sensor (raw)': 0
})
print("Final Step: 1023, Voltage: 3.800 V")
ser.close()
print("Sweep complete. Creating Excel file...")

df = pd.DataFrame(data_records)
excel_filename = "potentiometer_quadratic_sweep_with_current.xlsx"
df.to_excel(excel_filename, index=False)
print("Excel file created:", excel_filename)
