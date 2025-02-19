#include <EEPROM.h>

#define BAUD_RATE 2400
#define MAX_SIZE 1020
#define TIMEOUT 5000  // 5-second timeout

char buffer[MAX_SIZE];

void setup() {
    Serial.begin(BAUD_RATE);
    while (!Serial);  // Wait for Serial Monitor

    Serial.println("Arduino Ready. Waiting for PC data...");
}

void loop() {
    int index = 0;
    unsigned long lastReceivedTime = millis();

    // Wait for data from PC with timeout
    while ((millis() - lastReceivedTime) < TIMEOUT) {
        if (Serial.available() > 0) {
            char received = Serial.read();
            if (index < MAX_SIZE) {
                buffer[index++] = received;
            }
            lastReceivedTime = millis();
        }
    }

    // Store received data into EEPROM
    for (int i = 0; i < index; i++) {
        EEPROM.write(i, buffer[i]);
        delay(3);  // Small delay to allow EEPROM write
    }

    Serial.println("\nData storage complete.");
    Serial.print("Transmission Speed (PC → Arduino): ");
    Serial.print((index * 8) / (millis() / 1000.0));
    Serial.println(" bits/sec");

    delay(2000);

    // Start Transmission
    Serial.println("START_EEPROM");

    // Send stored data back to PC
    for (int i = 0; i < index; i++) {
        Serial.write(EEPROM.read(i));
        delayMicroseconds(600);  // Add small delay 
    }

    // End Transmission
    Serial.println("\nEND_EEPROM");

    while (1);
}
