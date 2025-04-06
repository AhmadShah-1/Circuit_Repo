import serial
import time
import pandas as pd

def main():
    # Serial port settings
    port = "/dev/ttyACM0"  # Update if necessary
    baud_rate = 9600
    excel_filename = "arduino_data.xlsx"

    try:
        ser = serial.Serial(port, baud_rate, timeout=1)
    except serial.SerialException as e:
        print(f"Error opening serial port {port}: {e}")
        return

    # Allow time for the Arduino to reset
    time.sleep(2)
    print("Connected to Arduino. Recording data...")

    # Data storage
    data_records = []
    start_time = time.time()

    try:
        while True:
            raw_line = ser.readline()
            if raw_line:
                try:
                    line = raw_line.decode('utf-8', errors='replace').strip()
                except UnicodeDecodeError as e:
                    print("Decode error:", e)
                    continue

                # Ensure correct format (two commas)
                if line.count(',') != 2:
                    print("Received malformed data:", line, "Raw:", raw_line.hex())
                    continue

                parts = line.split(',')
                if len(parts) == 3:
                    current_time, angle, rpm = parts
                    elapsed_time = time.time() - start_time  # Convert to seconds

                    # Store data in a structured format
                    data_records.append({
                        'Time (s)': elapsed_time,
                        'Angle': angle,
                        'RPM': rpm
                    })

                    print(f"Time: {elapsed_time:.2f} s, Angle: {angle}, RPM: {rpm}")

    except KeyboardInterrupt:
        print("Exiting the program.")
    finally:
        ser.close()
        if data_records:
            # Save data to an Excel file
            df = pd.DataFrame(data_records)
            df.to_excel(excel_filename, index=False)
            print(f"Excel file created: {excel_filename}")
        else:
            print("No data recorded.")

if __name__ == "__main__":
    main()
