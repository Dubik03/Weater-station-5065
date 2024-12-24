# Arduino RF433MHz Signal Reader & Transmitter
*For Weather station 5065*

---

## Overview
This project is designed for Arduino boards to receive data from the Weather station 5065 sensor and transmit data to the weather station using RF433MHz signals. The program allows you to capture weather data transmitted by the sensor (such as channel, temperature, humidity, and sensor ID) and also send custom data back to the station.

The program uses precise timing and signal duration analysis to decode incoming RF signals from the Weather station 5065 sensor and transmit custom data to the weather station.

---

## Features
- **RF Signal Reception**: Captures and decodes signals transmitted by the Weather station 5065 sensor.
- **Data Parsing**: Extracts and decodes sensor ID, temperature, humidity, and channel information.
- **Signal Analysis**: Utilizes signal timing and duration for decoding, with tolerance values for various signal components.
- **Custom Data Transmission**: Transmit binary data (such as channel, sensor ID, temperature, humidity) to the weather station using a custom protocol.
- **Display Mode**: Toggle raw data output to visually debug signal timings and bit encoding.

---

## Key Concepts and Signal Analysis
The program decodes the signals based on the length and timing of individual bits and separators. The sensor transmits data multiple times in bursts. Here's a detailed explanation of the steps involved:

### 1. Signal Reception
The Arduino listens to a digital pin (SIGNAL_PIN) for incoming signals. The signal is captured using an interrupt (dataHandler()), which logs timestamp data in microseconds.

### 2. Signal Decoding
The program utilizes timing data to interpret the signals. It looks for specific patterns that mark:

- **Sync Sequence**: A unique pattern that marks the beginning of the transmission.
- **Separator**: A brief pause indicating the end of one bit and the beginning of the next.
- **Bit Encoding**: The data is transmitted as a series of high or low signals, where the length of the signal determines whether it's a bit '1' or '0'.
- **Data Stream**: The raw data stream includes information on sensor ID, channel, temperature, and humidity.

### 3. Time Tolerances
The program uses several time-based thresholds to determine which signal represents which bit or separator:

| Parameter          | Description                     |
|--------------------|---------------------------------|
| `SYNC_PATTERN_LEN` | The duration of the sync signal.|
| `SEPARATOR_LEN`    | The length of a separator signal.|
| `BIT_1_LEN`        | The duration for a bit '1'.     |
| `BIT_0_LEN`        | The duration for a bit '0'.     |

Tolerances are applied to handle slight variations in signal timings.

### 4. Data Extraction
Once a valid signal is captured, the data is decoded:
- **Sensor ID**: A unique identifier for the sensor.(The ID will change when power is lost... battery replacement...)
- **Channel**: The communication channel of the sensor.
- **Temperature**: The sensor's temperature reading, including handling negative temperatures using 2's complement encoding.
- **Humidity**: The humidity level reported by the sensor.

---

## Sending Data to the Weather Station
In addition to reading data from the Weather station 5065, this project allows for sending custom data to the weather station. This can be useful for testing, simulating sensor data, or controlling certain parameters.

### 1. Data Transmission Protocol
To send data to the weather station, we use a custom protocol that is synchronized with the station’s reception mechanism. The transmission involves the following steps:

- **Synchronization Pattern**: A predefined pattern is sent at the start of each data block to signal the beginning of the transmission.
- **Data Encoding**: The data (such as sensor ID, channel, temperature, and humidity) is encoded into a binary stream. Each bit is transmitted by varying the duration of the signal (a longer pulse for bit '1' and a shorter one for bit '0').
- **Repetition**: To ensure reliable reception by the weather station, the data is transmitted multiple times in bursts. This redundancy improves the likelihood of successful communication.
- **Signal Separator**: A small separator pulse is added between each bit to distinguish one bit from the next.

### 2. Sending Data Example
The program allows the user to input a binary string representing the data to be sent to the weather station. For example:

#### 001011011100000011110010111100100111  // Binary string for transmission


---

## Setup and Usage
### Hardware Setup
Connect the RF433MHz signal from the Meteostation 5065 sensor to the Arduino's `SIGNAL_PIN` (default pin 2). Ensure you have the necessary power and ground connections set up for the Arduino.

### Software Setup
1. **Upload the Code**: Load the Arduino sketch into your Arduino board using the Arduino IDE.
2. **Serial Monitor**: Open the serial monitor at a baud rate of 9600 to view the output.
3. **Toggle Raw Data**: Press the `r` key on the serial monitor to toggle the display of raw data (signal timings). If you need...

Once the Arduino detects a valid signal, you will see output like:
### Example Output
- **Sensor ID: 110101010**
- **Channel: CH1 - MAN**
- **Temperature: 22.5** 
- **Humidity: 65**

Or, if raw data is enabled:
- **Raw Data (timings):**
- **2950 4000 6000 1900 ...**
- **Decoded Bits:**
- **1 0 1 SEP 1 0 ...**

---

## Code Breakdown
### checkSync()
This function checks whether the incoming signal matches the expected sync pattern. It compares the duration of two consecutive timestamps to ensure the correct pattern and synchronization.

### dataHandler()
An interrupt-based handler that captures signal transitions and stores the timestamp in a ring buffer. It then checks for a valid sync sequence and calculates whether a complete data packet has been received.

### decodeInfo()
This function is responsible for decoding the actual information (sensor ID, temperature, humidity, etc.). It uses bit-level decoding to extract the individual data values from the raw timing data.

### decodeTemperatureSign()
This function handles the sign of the temperature value, detecting whether the temperature is positive or negative by looking at specific signal patterns.

### calculateTemperature()
Converts the raw temperature data (from the sensor) to a human-readable Celsius value, handling both positive and negative temperatures appropriately.

### compareMeasurements()
This function allows multiple measurements of the same value to be compared. It filters out outliers based on the tolerance threshold and selects the most consistent reading.

---

## License
This code is released under the GNU General Public License v3.0, meaning you are free to modify, distribute, and use the code as long as you adhere to the terms of the license. For more information, see GNU GPLv3.

---

## Conclusion
This project allows you to interface an Arduino with an RF433MHz sensor, enabling you to capture and decode weather data transmitted by a Meteostation 5065 sensor. It demonstrates the use of signal timing analysis for decoding complex data streams and provides flexibility in displaying both raw and decoded sensor information.



