# Define the total duration for the sweep (in seconds)
T_total = 130.0  




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



data_records = []
start_time = time.time()
last_step = -1

print("Starting quadratic sweep from step 0 to 1023...")
while True:
    t = time.time() - start_time
    if t > T_total:
        break
    step = int(1023 * (t / T_total) ** 2)
    if step > 1023:
        step = 1023
    if step != last_step:
        voltage_out = (step / 1023.0) * 3.8
        elapsed_time = t
        data_records.append({
            'Time (s)': elapsed_time,
            'Step': step,
            'Voltage (V)': voltage_out
        })
        ser.write((str(step) + "\n").encode('utf-8'))
        print("Time: {:.2f} s, Step: {}, Voltage: {:.3f} V".format(elapsed_time, step, voltage_out))
        last_step = step
    time.sleep(0.05)

ser.write((str(1023) + "\n").encode('utf-8'))
data_records.append({
    'Time (s)': time.time() - start_time,
    'Step': 1023,
    'Voltage (V)': 3.8
})
print("Final Step: 1023, Voltage: 3.800 V")

ser.close()
print("Sweep complete. Creating Excel file...")

df = pd.DataFrame(data_records)
excel_filename = "quadratic.xlsx"
df.to_excel(excel_filename, index=False)

print("Excel file created:", excel_filename)
