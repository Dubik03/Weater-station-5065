Arduino RF433MHz Signal Reader for Meteostation 5065 Sensor

Overview
This program is designed for Arduino boards to read and decode data signals from a RF433MHz sensor (specifically a Meteostation 5065 sensor). The program captures timestamp data on signal transitions, decodes the sensor’s unique ID, channel, temperature, and humidity values from the transmitted signal, and displays the results via a serial interface.

Features
RF Signal Reception: Captures and decodes signals transmitted by the Meteostation 5065 sensor.
Data Parsing: Extracts and decodes sensor ID, temperature, humidity, and channel information.
Signal Analysis: Utilizes signal timing and duration for decoding, with tolerance values for various signal components.
Display Mode: Toggle raw data output to visually debug signal timings and bit encoding.
Key Concepts and Signal Analysis
The program decodes the signals based on the length and timing of individual bits and separators. The sensor transmits data multiple times in bursts. Here's a detailed explanation of the steps involved:

1. Signal Reception
The Arduino listens to a digital pin (SIGNAL_PIN) for incoming signals.
The signal is captured using an interrupt (dataHandler()), which logs timestamp data in microseconds.
2. Signal Decoding
The program utilizes timing data to interpret the signals. It looks for specific patterns that mark:
Sync Sequence: A unique pattern that marks the beginning of the transmission.
Separator: A brief pause indicating the end of one bit and the beginning of the next.
Bit Encoding: The data is transmitted as a series of high or low signals, where the length of the signal determines whether it's a bit '1' or '0'.
Data Stream: The raw data stream includes information on sensor ID, channel, temperature, and humidity.
3. Time Tolerances
The program uses several time-based thresholds to determine which signal represents which bit or separator:

SYNC_PATTERN_LEN: The duration of the sync signal.
SEPARATOR_LEN: The length of a separator signal.
BIT_1_LEN: The duration for a bit '1'.
BIT_0_LEN: The duration for a bit '0'.
Sync Tolerance: A tolerance value to allow slight variations in timing.
Separator Tolerance: A tolerance value for the separator duration.
Bit Length Tolerance: Tolerances are applied to both '1' and '0' bit lengths to allow for slight fluctuations.
4. Data Extraction
Once a valid signal is captured, the data is decoded:

Sensor ID: A unique identifier for the sensor.
Channel: The communication channel of the sensor.
Temperature: The sensor's temperature reading, including handling negative temperatures using 2's complement encoding.
Humidity: The humidity level reported by the sensor.
5. Data Display
The decoded data can be displayed in raw form (showing exact timings) or in a user-friendly format, showing:

Sensor ID in binary.
Channel identification (MANUAL/AUTO).
Temperature (in degrees Celsius).
Humidity percentage.


Arduino RF433MHz Signal Reader for Meteostation 5065 Sensor
Overview
This program is designed for Arduino boards to read and decode data signals from a RF433MHz sensor (specifically a Meteostation 5065 sensor). The program captures timestamp data on signal transitions, decodes the sensor’s unique ID, channel, temperature, and humidity values from the transmitted signal, and displays the results via a serial interface.

Features
RF Signal Reception: Captures and decodes signals transmitted by the Meteostation 5065 sensor.
Data Parsing: Extracts and decodes sensor ID, temperature, humidity, and channel information.
Signal Analysis: Utilizes signal timing and duration for decoding, with tolerance values for various signal components.
Display Mode: Toggle raw data output to visually debug signal timings and bit encoding.
Key Concepts and Signal Analysis
The program decodes the signals based on the length and timing of individual bits and separators. The sensor transmits data multiple times in bursts. Here's a detailed explanation of the steps involved:

1. Signal Reception
The Arduino listens to a digital pin (SIGNAL_PIN) for incoming signals.
The signal is captured using an interrupt (dataHandler()), which logs timestamp data in microseconds.
2. Signal Decoding
The program utilizes timing data to interpret the signals. It looks for specific patterns that mark:
Sync Sequence: A unique pattern that marks the beginning of the transmission.
Separator: A brief pause indicating the end of one bit and the beginning of the next.
Bit Encoding: The data is transmitted as a series of high or low signals, where the length of the signal determines whether it's a bit '1' or '0'.
Data Stream: The raw data stream includes information on sensor ID, channel, temperature, and humidity.
3. Time Tolerances
The program uses several time-based thresholds to determine which signal represents which bit or separator:

SYNC_PATTERN_LEN: The duration of the sync signal.
SEPARATOR_LEN: The length of a separator signal.
BIT_1_LEN: The duration for a bit '1'.
BIT_0_LEN: The duration for a bit '0'.
Sync Tolerance: A tolerance value to allow slight variations in timing.
Separator Tolerance: A tolerance value for the separator duration.
Bit Length Tolerance: Tolerances are applied to both '1' and '0' bit lengths to allow for slight fluctuations.
4. Data Extraction
Once a valid signal is captured, the data is decoded:

Sensor ID: A unique identifier for the sensor.
Channel: The communication channel of the sensor.
Temperature: The sensor's temperature reading, including handling negative temperatures using 2's complement encoding.
Humidity: The humidity level reported by the sensor.
5. Data Display
The decoded data can be displayed in raw form (showing exact timings) or in a user-friendly format, showing:

Sensor ID in binary.
Channel identification (MANUAL/AUTO).
Temperature (in degrees Celsius).
Humidity percentage.
Setup and Usage
Hardware Setup
Connect the RF433MHz signal from the Meteostation 5065 sensor to the Arduino's SIGNAL_PIN (default pin 2).
Ensure you have the necessary power and ground connections set up for the Arduino.
Software Setup
Upload the Code: Load the Arduino sketch into your Arduino board using the Arduino IDE.
Serial Monitor: Open the serial monitor at a baud rate of 9600 to view the output.
Toggle Raw Data: Press the 'r' key on the serial monitor to toggle the display of raw data (signal timings).
Example Output
Once the Arduino detects a valid signal, you will see output like:

yaml
Zkopírovat kód
Sensor ID: 1101010101010101
Channel: CH1 - MAN
Temperature: 22.5
Humidity: 65
Or, if raw data is enabled:

yaml
Zkopírovat kód
Raw Data (timings):
2950 4000 6000 1900 ...
Decoded Bits:
1 0 1 SEP 1 0 ...
Code Breakdown
checkSync()
This function checks whether the incoming signal matches the expected sync pattern. It compares the duration of two consecutive timestamps to ensure the correct pattern and synchronization.

dataHandler()
An interrupt-based handler that captures signal transitions and stores the timestamp in a ring buffer. It then checks for a valid sync sequence and calculates whether a complete data packet has been received.

decodeInfo()
This function is responsible for decoding the actual information (sensor ID, temperature, humidity, etc.). It uses bit-level decoding to extract the individual data values from the raw timing data.

decodeTemperatureSign()
This function handles the sign of the temperature value, detecting whether the temperature is positive or negative by looking at specific signal patterns.

calculateTemperature()
Converts the raw temperature data (from the sensor) to a human-readable Celsius value, handling both positive and negative temperatures appropriately.

compareMeasurements()
This function allows multiple measurements of the same value to be compared. It filters out outliers based on the tolerance threshold and selects the most consistent reading.

License
This code is released under the GNU General Public License v3.0, meaning you are free to modify, distribute, and use the code as long as you adhere to the terms of the license. For more information, see GNU GPLv3.

Conclusion
This project allows you to interface an Arduino with an RF433MHz sensor, enabling you to capture and decode weather data transmitted by a Meteostation 5065 sensor. It demonstrates the use of signal timing analysis for decoding complex data streams and provides flexibility in displaying both raw and decoded sensor information.
