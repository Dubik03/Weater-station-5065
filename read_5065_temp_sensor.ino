/*
 * This code is licensed under the GNU General Public License v3.0.
 * 
 * Author: Dubik
 * Description: This code is designed to read data from a Meteostation 5065 sensor. It captures timestamps for signal transitions and decodes Sensor ID, temperature, channel and humidity values from the sensor's data stream.
 * 
 * License: This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#define BUFFER_SIZE      256
#define SYNC_PATTERN_LEN 3900
#define SYNC_TOLERANCE   200
#define SEPARATOR_LEN    580
#define BIT_1_LEN        1900
#define BIT_0_LEN        920
#define SIGNAL_PIN       2  // Pin for signal

// Tolerance for decoding
float syncTolerance = 200.0;
float separatorTolerance = 50.0;
float bit1Tolerance = 100.0;
float bit0Tolerance = 100.0;

bool displayRawData = false;  // Flag for displaying raw data
unsigned long timestampBuffer[BUFFER_SIZE];  // Ring buffer for storing timestamps
unsigned int syncPos1 = 0, syncPos2 = 0;  // Indices for sync sequences
bool dataReceived = false;  // Flag to confirm data reception

// Function to check the sync sequence
bool checkSync(unsigned int idx) {
  unsigned long prevTime = timestampBuffer[(idx + BUFFER_SIZE - 1) % BUFFER_SIZE];
  unsigned long currentTime = timestampBuffer[idx];

  return (prevTime > (SEPARATOR_LEN - separatorTolerance) && prevTime < (SEPARATOR_LEN + separatorTolerance) &&
          currentTime > (SYNC_PATTERN_LEN - syncTolerance) && currentTime < (SYNC_PATTERN_LEN + syncTolerance) &&
          digitalRead(SIGNAL_PIN) == HIGH);
}

// Interrupt handler and data collection
void dataHandler() {
  static unsigned long timeElapsed = 0;
  static unsigned long previousTime = 0;
  static unsigned int bufferIndex = 0;
  static unsigned int syncCount = 0;

  if (dataReceived) return;

  long currentTime = micros();
  timeElapsed = currentTime - previousTime;
  previousTime = currentTime;

  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
  timestampBuffer[bufferIndex] = timeElapsed;

  if (checkSync(bufferIndex)) {
    syncCount++;
    if (syncCount == 1) {
      syncPos1 = (bufferIndex + 1) % BUFFER_SIZE;
    } else if (syncCount == 2) {
      syncCount = 0;
      syncPos2 = (bufferIndex + 1) % BUFFER_SIZE;
      unsigned int interval = (syncPos2 < syncPos1) ? (syncPos2 + BUFFER_SIZE - syncPos1) : (syncPos2 - syncPos1);
      dataReceived = (interval == 74);
      if (!dataReceived) {
        syncPos1 = syncPos2 = 0;
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Started.");
  pinMode(SIGNAL_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), dataHandler, CHANGE);
  Serial.println("Use 'r' to toggle raw data display.");
}

void loop() {
  if (dataReceived) {
    detachInterrupt(digitalPinToInterrupt(SIGNAL_PIN));

    unsigned long sensorID = 0, channel = 0;
    bool isNegativeTemp = false;
    int rawTemperature = 0, rawHumidity = 0;
    float temperature = 0.0, humidity = 0.0;

    // Display raw data if enabled
    if (displayRawData) {
      Serial.println("Raw Data (timings):");
      for (unsigned int i = syncPos1; i != syncPos2; i = (i + 1) % BUFFER_SIZE) {
        Serial.print(timestampBuffer[i]);
        Serial.print(" ");
      }
      Serial.println();

      Serial.println("Decoded Bits:");
      for (unsigned int i = syncPos1; i != syncPos2; i = (i + 1) % BUFFER_SIZE) {
        if (timestampBuffer[i] > (SEPARATOR_LEN - bit0Tolerance) && timestampBuffer[i] < (SEPARATOR_LEN + bit0Tolerance)) {
          Serial.print("SEP ");
        } else if (timestampBuffer[i] > (BIT_1_LEN - bit1Tolerance) && timestampBuffer[i] < (BIT_1_LEN + bit1Tolerance)) {
          Serial.print("1 ");
        } else if (timestampBuffer[i] > (BIT_0_LEN - bit0Tolerance) && timestampBuffer[i] < (BIT_0_LEN + bit0Tolerance)) {
          Serial.print("0 ");
        } else {
          Serial.print("? ");
        }
      }
      Serial.println();
    }

    // Decode sensor ID
    sensorID = decodeInfo(syncPos1, 18);
    Serial.print("Sensor ID: ");
    Serial.println(sensorID, BIN);

    // Decode channel
    channel = decodeInfo((syncPos1 + 18) % BUFFER_SIZE, 6);
    Serial.print("Channel: ");
    printChannel(channel);

    // Decode temperature (with sign)
    isNegativeTemp = decodeTemperatureSign(syncPos1 + 24);
    rawTemperature = decodeInfo(syncPos1 + 30, 18);
    temperature = calculateTemperature(isNegativeTemp, rawTemperature);

    Serial.print("Temperature: ");
    Serial.println(temperature);

    // Decode humidity
    rawHumidity = decodeInfo(syncPos1 + 56, 18);
    Serial.print("Humidity: ");
    Serial.println(rawHumidity);

    // Reset state and enable interrupt for next reading
    dataReceived = false;
    syncPos1 = syncPos2 = 0;
    attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), dataHandler, CHANGE);
  }

  // Toggle raw data display with 'r'
  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'r') {
      displayRawData = !displayRawData;
      Serial.print("Raw data display: ");
      Serial.println(displayRawData ? "ON" : "OFF");
    }
  }
}

// Decode data (ID, channel, temperature, humidity)
unsigned long decodeInfo(unsigned int startIdx, unsigned int length) {
  unsigned long value = 0;
  for (unsigned int i = startIdx; i != (startIdx + length) % BUFFER_SIZE; i = (i + 2) % BUFFER_SIZE) {
    unsigned long time0 = timestampBuffer[i], time1 = timestampBuffer[(i + 1) % BUFFER_SIZE];
    if (time0 > (SEPARATOR_LEN - bit0Tolerance) && time0 < (SEPARATOR_LEN + bit0Tolerance)) {
      if (time1 > (BIT_1_LEN - bit1Tolerance) && time1 < (BIT_1_LEN + bit1Tolerance)) {
        value = (value << 1) | 1;
      } else if (time1 > (BIT_0_LEN - bit0Tolerance) && time1 < (BIT_0_LEN + bit0Tolerance)) {
        value = (value << 1) | 0;
      }
    }
  }
  return value;
}

// Print channel information
void printChannel(unsigned long channel) {
  switch (channel) {
    case 0b100: Serial.println("CH1 - MAN"); break;
    case 0b000: Serial.println("CH1 - AUT"); break;
    case 0b101: Serial.println("CH2 - MAN"); break;
    case 0b001: Serial.println("CH2 - AUT"); break;
    case 0b110: Serial.println("CH3 - MAN"); break;
    case 0b010: Serial.println("CH3 - AUT"); break;
    default: Serial.println("Unknown Channel"); break;
  }
}

// Decode the signal for temperature sign
bool decodeTemperatureSign(unsigned int startIdx) {
  unsigned int tempSignBits = 0;
  for (unsigned int i = startIdx; i != (startIdx + 8) % BUFFER_SIZE; i = (i + 2) % BUFFER_SIZE) {
    unsigned long time0 = timestampBuffer[i], time1 = timestampBuffer[(i + 1) % BUFFER_SIZE];
    if (time0 > (SEPARATOR_LEN - bit0Tolerance) && time0 < (SEPARATOR_LEN + bit0Tolerance)) {
      if (time1 > (BIT_1_LEN - bit1Tolerance) && time1 < (BIT_1_LEN + bit1Tolerance)) {
        tempSignBits = (tempSignBits << 1) | 1;
      } else if (time1 > (BIT_0_LEN - bit0Tolerance) && time1 < (BIT_0_LEN + bit0Tolerance)) {
        tempSignBits = (tempSignBits << 1) | 0;
      }
    }
  }
  return (tempSignBits == 0b1111);
}

// Calculate temperature
float calculateTemperature(bool isNegative, int rawValue) {
  if (isNegative) {
    return ((int8_t)rawValue) / 10.0;  // Negative temperature using 2's complement
  }
  return rawValue / 10.0;
}

// Compare multiple measurements and discard invalid ones
float compareMeasurements(float measurements[], unsigned int numMeasurements) {
  // Count valid measurements
  unsigned int validCount = 0;
  float validTemperature = NAN;
  
  for (unsigned int i = 0; i < numMeasurements; i++) {
    if (isnan(measurements[i])) continue;  // Skip invalid measurements
    unsigned int matchingCount = 0;
    
    // Compare with all other measurements
    for (unsigned int j = 0; j < numMeasurements; j++) {
      if (i != j && fabs(measurements[i] - measurements[j]) <= 0.5) {  // Tolerance for comparison
        matchingCount++;
      }
    }
    
    if (matchingCount >= numMeasurements / 2) {  // If at least half match
      validTemperature = measurements[i];
      validCount++;
    }
  }
  
  if (validCount >= 2) {
    return validTemperature;
  } else {
    return NAN;  // No valid temperature
  }
}
