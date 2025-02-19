#include <EEPROM.h>

#define BAUD_RATE 2400
#define MAX_SIZE 1020
#define TIMEOUT 5000  

char buffer[MAX_SIZE];

void setup() {
    Serial.begin(BAUD_RATE);
    while (!Serial);  

    Serial.println("Arduino Ready. Waiting for PC data...");
}

void loop() {
    int index = 0;
    unsigned long lastReceivedTime = millis();

  
    while ((millis() - lastReceivedTime) < TIMEOUT) {
        if (Serial.available() > 0) {
            char received = Serial.read();
            if (index < MAX_SIZE) {
                buffer[index++] = received;
            }
            lastReceivedTime = millis();
        }
    }

    for (int i = 0; i < index; i++) {
        EEPROM.write(i, buffer[i]);
        delay(3);  
    }

    Serial.println("\nData storage complete.");
    Serial.print("Transmission Speed (PC → Arduino): ");
    Serial.print((index * 8) / (millis() / 1000.0));
    Serial.println(" bits/sec");

    delay(2000);

    Serial.println("START_EEPROM");

    for (int i = 0; i < index; i++) {
        Serial.write(EEPROM.read(i));
        delayMicroseconds(600);  
    }

    Serial.println("\nEND_EEPROM");

    while (1);
}
