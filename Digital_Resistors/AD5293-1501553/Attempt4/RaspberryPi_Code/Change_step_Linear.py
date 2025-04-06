# 15 steps every 0.5 seconds
change_of_steps = 70
voltage_in = 5.06

import serial
import time
import pandas as pd
import re

SERIAL_PORT = '/dev/ttyACM1'
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
except serial.SerialException as e:
    print("Error opening serial port:", e)
    exit(1)

time.sleep(2)
print("Connected to Arduino on", SERIAL_PORT)

# Prepare to collect data for Excel
data_records = []  # each record: time, step, voltage

start_time = time.time()

for step in range(0, 1024, change_of_steps):
    voltage_out = (step / 1023.0) * voltage_in
    elapsed_time = time.time() - start_time
    ser.write((str(step) + "\n").encode('utf-8'))
    time.sleep(0.2)
    current_reading = None

    while ser.in_waiting:
        line = ser.readline().decode('utf-8', errors='replace').strip()
        m = re.search(r"Wiper set to:\s*(\d+)\s*V,\s*Calculated Current:\s*([\d\.]+)\s*A", line)
        if m:
            step_read = int(m.group(1))
            current_reading = float(m.group(2))  # Change to float to capture decimal values
            print("Arduino:", line)
        else:
            print("Arduino:", line)

    if current_reading is None:
        current_reading = 0.0  # Ensure correct type

    data_records.append({
        'Time (s)': elapsed_time,
        'Step': step,
        'Voltage (V)': voltage_out,
        'Current Sensor (A)': current_reading  # Now stores correct floating-point values
    })

    time.sleep(0.5)

ser.close()
print("Sweep complete. Creating Excel file...")

df = pd.DataFrame(data_records)
excel_filename = "Linear.xlsx"
df.to_excel(excel_filename, index=False)

print("Excel file created:", excel_filename)
