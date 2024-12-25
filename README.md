# Arduino RF433MHz Signal Reader & Transmitter
*For Weather station 5065 - Not working with weather station/sensor 5062*

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
The Arduino listens to a digital pin (SIGNAL_PIN) for incoming signals. The signal is captured using an interrupt (`dataHandler()`), which logs timestamp data in microseconds.

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

Binary string for transmission:
##### 001011011100000011110010111100100111

This string is then sent multiple times (10 repetitions) to the station.

#### Code for Transmitting Data:
- **Synchronization Signal**: A signal of fixed length marks the beginning of the transmission.
- **Bit Transmission**: Each bit ('0' or '1') is transmitted based on specific time durations.
- **Repetition**: The entire block of data is sent multiple times to ensure successful reception.

---

## Setup and Usage
### Hardware Setup

- **Receiver Setup**: Connect the RF433MHz receiver from the Weather station 5065 sensor to the Arduino’s SIGNAL_PIN (default pin 2). This allows the Arduino to listen for incoming signals from the sensor.
- **Transmitter Setup**: If you wish to transmit data back to the weather station, connect an RF433MHz transmitter to the Arduino’s defined transmission pin (e.g., pin 2 or another pin depending on your setup).
- Ensure that the power and ground connections are correctly set up for both the Arduino and RF devices.

### Software Setup
1. **Upload the Code**: Load the Arduino sketch into your Arduino board using the Arduino IDE.
2. **Serial Monitor**: Open the serial monitor at a baud rate of 9600 to view the output. This will allow you to interact with the system and view the decoded sensor data or input your own data for transmission.
3. **Toggle Raw Data (Optional)**: Press the 'r' key on the serial monitor to toggle the display of raw signal data (timing of bits) if you wish to debug or analyze the signal transmission.
4. **Input Binary Data for Transmission**: To send custom data to the weather station, you can input a binary string in the serial monitor (e.g., 001011011100000011110010111100100111) and the system will send this data in RF signal bursts.

### Reception of Data from the Sensor

1. **Signal Reception**:  The Arduino continuously listens for incoming signals from the Weather station 5065 sensor. When a valid signal is received, the program decodes the data to extract important weather information, such as:

- **Sensor ID**
- **Channel**
- **Temperature**
- **Humidity**

The decoded data is then printed to the serial monitor for display.

 Example Output
- **Sensor ID: 001011011**
- **Channel: CH1 - MAN**
- **Temperature: 24.2** 
- **Humidity: 39**

2. **Raw Data Output**: If you enabled raw data, you will see the timing of the received signals and the decoded bits. This can help in debugging the reception process or in analyzing the signal patterns.

Example raw data output:
- **Raw Data (timings):**
- **583 1982 592 932 581 1905 586 1887 588 928 ...**
- **Decoded Bits:**
- **SEP 1 SEP 0 SEP 1 SEP 1 SEP 0 ...**

### Transmission of Custom Data to the Weather Station
- **1. Prepare the Data**: Convert your data (such as a sensor ID, temperature, and humidity values) into a binary string format (e.g., 001011011100000011110010111100100111).


| Bits               | Description                        |     
|--------------------|------------------------------------|
| `001011011`        | Sensor ID                          |
| `100`              | Channel ID                         |
| `0000`            | 0000/1111 positive/negative temp.   |
| `11110010`        | Temperature                         |
| `1111`            | Temp/humidity separator             |
| `00100111`        | Humidity                            |

| Bits    | Channel ID   |     
|---------|--------------|
| `000`   | CH1 - AUT    |
| `100`   | CH1 - MAN    |
| `001`   |  CH2 - AU    |
| `101`   |  CH2 - MAN   |
| `010`   |  CH3 - AUT   |
| `110`   |  CH3 - MAN   |

AUT - automatic signal 

MAN - Signal sent by pressing a button on the sensor
  
     
  
Bits conversion to temperature... if positive =DEC / 10, if negative =(DEC-256)/10 

(0000)11110010... 242 / 10 == 24,2°C

(1111)11110010... (242-256) / 10 == -1,4°C


- **2.Transmit the Data**: After entering the binary string in the serial monitor, the Arduino will transmit the signal multiple times (10 repetitions) to the weather station. This ensures more reliable receiving by the station.

- **3.Transmission Protocol**: The data is sent with a synchronization pattern at the beginning, followed by the bit encoding of the data. The Arduino sends the binary bits, using precise timing to encode 1s and 0s, and the transmission is repeated multiple times to ensure reliability.

## Example Scenario: Receiving and Transmitting Data

- **1**. The Arduino receives the weather data from the Weather station 5065 sensor and decodes it.
- **2**. The decoded values (e.g., temperature, humidity, etc.) are displayed on the serial monitor.
- **3**. If you input a custom binary string (e.g., 001011011100000011110010111100100111), the Arduino transmits this data to the weather station using the custom signal protocol.

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

## Summary
Receive data from the Weather station 5065 sensor to display the weather information.Transmit custom binary data back to the weather station for testing or other purposes. View the decoded data and signal timings on the serial monitor for debugging or analysis.
By following these steps, you can easily interact with both the Weather station 5065 sensor and the weather station, receiving real-time weather data and transmitting custom signals.


This revised version combines both the reception and transmission setup and usage, with a clearer explanation of both processes in one section.

---

## License
This code is released under the GNU General Public License v3.0, meaning you are free to modify, distribute, and use the code as long as you adhere to the terms of the license. For more information, see GNU GPLv3.
