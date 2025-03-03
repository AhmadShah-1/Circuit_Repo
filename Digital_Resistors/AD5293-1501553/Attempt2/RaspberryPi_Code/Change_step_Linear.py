# 15 steps every 0.5 seconds

change_of_steps = 15
voltage_in = 3.8





import serial
import time
import pandas as pd

SERIAL_PORT = '/dev/ttyACM0'
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
        
    data_records.append({
        'Time (s)': elapsed_time,
        'Step': step,
        'Voltage (V)': voltage_out
    })
    
    time.sleep(0.5)

ser.close()
print("Sweep complete. Creating Excel file...")

df = pd.DataFrame(data_records)
excel_filename = "Linear.xlsx"
df.to_excel(excel_filename, index=False)

print("Excel file created:", excel_filename)
