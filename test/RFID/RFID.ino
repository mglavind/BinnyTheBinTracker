
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         22           // Configurable, see typical pin layout above
#define SS_PIN          5          // Configurable, see typical pin layout above
#define LED_PIN         12
#define APPEND_MODE_PIN 16  // Pin to toggle append mode
#define STANDARD_MODE_PIN 17  // Pin to toggle standard mode
#define RESET_COUNT_PIN 2     // Pin to reset UID count


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;
bool appendMode = false;  // Append mode flag
bool standardMode = true;  // Standard mode flag
byte knownUIDs[][4] = {  // List of known UIDs
  {0x12, 0x34, 0x56, 0x78},
  {0xAB, 0xCD, 0xEF, 0x01}
};
int numKnownUIDs = sizeof(knownUIDs) / sizeof(knownUIDs[0]);  // Number of known UIDs
int numUniqueUIDsScanned = 0;  // Number of unique UIDs scanned
bool resetCountFlag = false;  // Reset count flag
unsigned long resetCountStartTime = 0;  // Reset count start time
unsigned long resetCountDuration = 5000;  // Reset count duration in milliseconds (5 seconds)



// Function to print UID bytes in hexadecimal format
void printUID(byte* uid, byte uidSize) {
    for (byte i = 0; i < uidSize; i++) {
        if (uid[i] < 0x10) {
            Serial.print("0");  // Add leading zero if necessary
        }
        Serial.print(uid[i], HEX);
    }
}


void setup() {
    Serial.begin(115200);    // Initialize serial communication
    SPI.begin();             // Initialize SPI bus
    mfrc522.PCD_Init();      // Initialize MFRC522

    pinMode(APPEND_MODE_PIN, INPUT);         // Set append mode pin as input
    pinMode(STANDARD_MODE_PIN, INPUT);       // Set standard mode pin as input
    pinMode(RESET_COUNT_PIN, INPUT);         // Set reset count pin as input
}

void loop() {
    // Check append mode pin
    if (digitalRead(APPEND_MODE_PIN) == HIGH) {
        appendMode = true;
        standardMode = false;
    }

    // Check standard mode pin
    if (digitalRead(STANDARD_MODE_PIN) == HIGH) {
        appendMode = false;
        standardMode = true;
    }

    // Check reset count pin
    if (digitalRead(RESET_COUNT_PIN) == HIGH) {
        resetCountFlag = true;
        resetCountStartTime = millis();
        Serial.println("Dette fungerer ikke");
    } else {
        resetCountFlag = false;
    }

    // Check if reset count duration has elapsed
    if (resetCountFlag && (millis() - resetCountStartTime >= resetCountDuration)) {
        numUniqueUIDsScanned = 0;  // Reset count for unique UIDs
        Serial.println("UID count reset!");
    }

// Check for RFID card
if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (standardMode) {
        // Standard mode: Check UID against known UIDs and increment count for unique UIDs
        bool uidMatched = false;
        for (int i = 0; i < numKnownUIDs; i++) {
            if (memcmp(mfrc522.uid.uidByte, knownUIDs[i], mfrc522.uid.size) == 0) {
                uidMatched = true;
                Serial.print("UID matched with known UID #");
                Serial.println(i + 1);
                break;
            }
        }

        if (!uidMatched) {
            numUniqueUIDsScanned++;  // Increment count for unique UIDs
            Serial.print("UID scanned: ");
            printUID(mfrc522.uid.uidByte, mfrc522.uid.size);
            Serial.print("   Unique UID count: ");
            Serial.println(numUniqueUIDsScanned);
        }
    } else if (appendMode) {
        // Append mode: Check UID against known UIDs and append to list if not found
        bool uidMatched = false;
        for (int i = 0; i < numKnownUIDs; i++) {
            if (memcmp(mfrc522.uid.uidByte, knownUIDs[i], mfrc522.uid.size) == 0) {
                uidMatched = true;
                Serial.print("UID matched with known UID #");
                Serial.println(i + 1);
                break;
            }
        }

        if (!uidMatched) {
            // Append UID to list of known UIDs
            memcpy(knownUIDs[numKnownUIDs], mfrc522.uid.uidByte, mfrc522.uid.size);
            numKnownUIDs++;
            Serial.print("UID appended to known UIDs: ");
            printUID(mfrc522.uid.uidByte, mfrc522.uid.size);
            Serial.print("   New known UID count: ");
            Serial.println(numKnownUIDs);
        }
    }
    mfrc522.PICC_HaltA();  // Halt PICC
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
    }
}

