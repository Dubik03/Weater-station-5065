/*
 * This code is licensed under the GNU General Public License v3.0.
 * 
 * Author: Dubik
 * Description: This code transmits binary data to the Meteostation 5065 (not compatible with 5062).
 * The data includes sensor ID, channel, temperature, and humidity values.
 * The transmission protocol uses synchronization patterns, bit separators, and specific durations for '0' and '1'.
 * To ensure reliable communication, the data is sent multiple times.
 * 
 * License: This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with this program. 
 * If not, see <https://www.gnu.org/licenses/>.
 */

#define SIGNAL_PIN 2  // Signal output pin

// Protocol timing parameters in microseconds
#define SYNC_PATTERN_LEN 3900  // Duration of the synchronization pattern
#define SEPARATOR_LEN    580   // Duration of the separator between bits
#define BIT_1_LEN        1900  // Duration for bit '1'
#define BIT_0_LEN        920   // Duration for bit '0'

/**
 * Sends the synchronization signal to prepare the receiver.
 */
void sendSync() {
  digitalWrite(SIGNAL_PIN, LOW);
  delayMicroseconds(SYNC_PATTERN_LEN);
  digitalWrite(SIGNAL_PIN, HIGH);  // Return to idle HIGH state
}

/**
 * Sends a single bit ('0' or '1') based on the specified timing.
 * @param bit Character representing the bit to send ('0' or '1').
 */
void sendBit(char bit) {
  // Send a separator (always HIGH for a fixed duration)
  digitalWrite(SIGNAL_PIN, HIGH);
  delayMicroseconds(SEPARATOR_LEN);

  // Set LOW for the duration of the bit
  digitalWrite(SIGNAL_PIN, LOW);
  if (bit == '1') {
    delayMicroseconds(BIT_1_LEN);
  } else if (bit == '0') {
    delayMicroseconds(BIT_0_LEN);
  }
  digitalWrite(SIGNAL_PIN, HIGH);  // Return to HIGH after the bit
}

/**
 * Sends a binary data string with a preceding synchronization signal.
 * @param data Pointer to a null-terminated binary string (e.g., "101010").
 */
void sendData(const char* data) {
  sendSync();  // Send synchronization signal

  // Transmit each bit in the data string
  while (*data) {
    if (*data == '1' || *data == '0') {
      sendBit(*data);
    }
    data++;
  }

  // Add a separator at the end of the data block
  digitalWrite(SIGNAL_PIN, HIGH);
  delayMicroseconds(SEPARATOR_LEN);
}

/**
 * Repeats the transmission of the given binary data string a specified number of times.
 * @param data Pointer to a null-terminated binary string.
 * @param repetitions Number of times the data should be transmitted.
 */
void sendRepeatedData(const char* data, int repetitions) {
  for (int i = 0; i < repetitions; i++) {
    sendData(data);
  }
  // Ensure the signal remains HIGH at the end of transmission
  digitalWrite(SIGNAL_PIN, HIGH);
}

/**
 * Arduino setup function. Initializes the serial communication and signal pin.
 */
void setup() {
  Serial.begin(9600);                 // Initialize serial communication
  pinMode(SIGNAL_PIN, OUTPUT);        // Set signal pin as output
  digitalWrite(SIGNAL_PIN, LOW);      // Default state is LOW

  Serial.println("Meteostation transmitter started.");
  Serial.println("Enter a binary string to transmit:");
}

/**
 * Arduino main loop. Waits for user input from the serial monitor and sends the entered binary string.
 */
void loop() {
  if (Serial.available()) {
    char data[256] = {0};              // Buffer for the input data
    Serial.readBytesUntil('\n', data, sizeof(data) - 1);

    Serial.print("Transmitting: ");
    Serial.println(data);

    // Transmit the data 10 times for reliability
    sendRepeatedData(data, 10);

    Serial.println("Transmission completed.");
  }
}
