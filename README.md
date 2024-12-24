<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Arduino RF433MHz Signal Reader</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>
    <header>
        <h1>Arduino RF433MHz Signal Reader</h1>
        <p>For Meteostation 5065 Sensor</p>
    </header>
    <div class="container">
        <h2>Overview</h2>
        <p>
            This program is designed for Arduino boards to read and decode data signals from a RF433MHz sensor 
            (specifically a Meteostation 5065 sensor). The program captures timestamp data on signal transitions, 
            decodes the sensor’s unique ID, channel, temperature, and humidity values from the transmitted signal, 
            and displays the results via a serial interface.
        </p>

        <h2>Features</h2>
        <ul>
            <li><strong>RF Signal Reception</strong>: Captures and decodes signals transmitted by the Meteostation 5065 sensor.</li>
            <li><strong>Data Parsing</strong>: Extracts and decodes sensor ID, temperature, humidity, and channel information.</li>
            <li><strong>Signal Analysis</strong>: Utilizes signal timing and duration for decoding, with tolerance values for various signal components.</li>
            <li><strong>Display Mode</strong>: Toggle raw data output to visually debug signal timings and bit encoding.</li>
        </ul>

        <h2>Key Concepts and Signal Analysis</h2>
        <p>
            The program decodes the signals based on the length and timing of individual bits and separators. 
            The sensor transmits data multiple times in bursts. Here's a detailed explanation of the steps involved:
        </p>
        <h3>1. Signal Reception</h3>
        <p>
            The Arduino listens to a digital pin (<code>SIGNAL_PIN</code>) for incoming signals. 
            The signal is captured using an interrupt (<code>dataHandler()</code>), which logs timestamp data in microseconds.
        </p>

        <h3>2. Signal Decoding</h3>
        <p>
            The program utilizes timing data to interpret the signals. It looks for specific patterns that mark:
        </p>
        <ul>
            <li><strong>Sync Sequence</strong>: A unique pattern that marks the beginning of the transmission.</li>
            <li><strong>Separator</strong>: A brief pause indicating the end of one bit and the beginning of the next.</li>
            <li><strong>Bit Encoding</strong>: The data is transmitted as a series of high or low signals, 
                where the length of the signal determines whether it's a bit '1' or '0'.</li>
            <li><strong>Data Stream</strong>: The raw data stream includes information on sensor ID, channel, temperature, and humidity.</li>
        </ul>

        <h3>3. Time Tolerances</h3>
        <p>
            The program uses several time-based thresholds to determine which signal represents which bit or separator:
        </p>
        <pre>
SYNC_PATTERN_LEN: The duration of the sync signal.
SEPARATOR_LEN: The length of a separator signal.
BIT_1_LEN: The duration for a bit '1'.
BIT_0_LEN: The duration for a bit '0'.
Sync Tolerance: A tolerance value to allow slight variations in timing.
Separator Tolerance: A tolerance value for the separator duration.
Bit Length Tolerance: Tolerances are applied to both '1' and '0' bit lengths to allow for slight fluctuations.
        </pre>

        <h3>4. Data Extraction</h3>
        <p>
            Once a valid signal is captured, the data is decoded:
        </p>
        <ul>
            <li><strong>Sensor ID</strong>: A unique identifier for the sensor.</li>
            <li><strong>Channel</strong>: The communication channel of the sensor.</li>
            <li><strong>Temperature</strong>: The sensor's temperature reading, including handling negative temperatures using 2's complement encoding.</li>
            <li><strong>Humidity</strong>: The humidity level reported by the sensor.</li>
        </ul>

        <h2>Setup and Usage</h2>
        <h3>Hardware Setup</h3>
        <p>
            Connect the RF433MHz signal from the Meteostation 5065 sensor to the Arduino's 
            <code>SIGNAL_PIN</code> (default pin 2). Ensure you have the necessary power and ground connections set up for the Arduino.
        </p>

        <h3>Software Setup</h3>
        <ol>
            <li><strong>Upload the Code</strong>: Load the Arduino sketch into your Arduino board using the Arduino IDE.</li>
            <li><strong>Serial Monitor</strong>: Open the serial monitor at a baud rate of 9600 to view the output.</li>
            <li><strong>Toggle Raw Data</strong>: Press the 'r' key on the serial monitor to toggle the display of raw data (signal timings).</li>
        </ol>

        <h3>Example Output</h3>
        <pre>
Sensor ID: 110101010
Channel: CH1 - MAN
Temperature: 22.5
Humidity: 65
        </pre>

        <h2>License</h2>
        <p>
            This code is released under the <strong>GNU General Public License v3.0</strong>, meaning you are free to modify, distribute, 
            and use the code as long as you adhere to the terms of the license. For more information, see 
            <a href="https://www.gnu.org/licenses/gpl-3.0.html" target="_blank">GNU GPLv3</a>.
        </p>
    </div>
    <footer class="footer">
        <p>© 2024 - Arduino RF433MHz Signal Reader Documentation</p>
    </footer>
</body>
</html>

