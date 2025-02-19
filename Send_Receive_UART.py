import serial
import time

SERIAL_PORT = "COM3"
BAUD_RATE = 2400
TEXT_TO_SEND = """Finance Minister Arun Jaitley Tuesday hit out at former RBI governor Raghuram Rajan for predicting 
that the next banking crisis would be triggered by MSME lending, saying postmortem is easier than taking action 
when it was required. Rajan, who had as the chief economist at IMF warned of impending financial crisis of 2008, 
in a note to a parliamentary committee warned against ambitious credit targets and loan waivers, saying that they 
could be the sources of next banking crisis. Government should focus on sources of the next crisis, not just the 
last one. In particular, government should refrain from setting ambitious credit targets or waiving loans. Credit 
targets are sometimes achieved by abandoning appropriate due diligence, creating the environment for future NPAs," 
Rajan said in the note." Both MUDRA loans as well as the Kisan Credit Card, while popular, have to be examined 
more closely for potential credit risk. Rajan, who was RBI governor for three years till September 2016, is currently"""

with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=10) as ser:
    ser.reset_input_buffer()  
    print("Sending data to Arduino...")
    start_time = time.time()
    ser.write(TEXT_TO_SEND.encode())  
    ser.flush()
    end_time = time.time()
    transmission_time = end_time - start_time
    transmission_speed = (len(TEXT_TO_SEND) * 8) / transmission_time
    print(f"Sent data. Speed: {transmission_speed:.2f} bits/sec")

    print("\nWaiting for EEPROM data from Arduino...")
    while "START_EEPROM" not in ser.readline().decode(errors="ignore"):
        pass

    start_time = time.time()
    received_data = ser.read_until(b"END_EEPROM").replace(b"END_EEPROM", b"")
    end_time = time.time()
    reception_time = end_time - start_time
    reception_speed = (len(received_data) * 8) / reception_time

    print("\nEEPROM Data Received:\n", received_data.decode(errors="ignore"))
    print(f"Reception speed: {transmission_speed:.2f} bits/sec")