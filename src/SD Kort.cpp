#include <SPI.h>
#include <SD.h>
#include <MFRC522.h>

#define MFRC522_SS_PIN 5
#define SD_SS_PIN 4
#define RST_PIN 22

MFRC522 mfrc522;

File myFile;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(MFRC522_SS_PIN, OUTPUT);
  digitalWrite(MFRC522_SS_PIN, HIGH);

  pinMode(SD_SS_PIN, OUTPUT);
  digitalWrite(SD_SS_PIN, HIGH);

  if (!SD.begin(SD_SS_PIN)) {
    Serial.println("SD Card failed");
    return;
  }

  Serial.println("SD Card initialized");
}

void loop() {
  digitalWrite(SD_SS_PIN, HIGH);
  digitalWrite(MFRC522_SS_PIN, LOW);
  
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    byte uid[4];
    for (byte i = 0; i < 4; i++) {
      uid[i] = mfrc522.uid.uidByte[i];
    }

    digitalWrite(SD_SS_PIN, LOW);
    digitalWrite(MFRC522_SS_PIN, HIGH);
    myFile = SD.open("/uid.txt", FILE_WRITE);
    if (myFile) {
      for (byte i = 0; i < 4; i++) {
        myFile.print(uid[i], HEX);
      }
      myFile.println();
      myFile.close();
      Serial.println("UID saved to SD card");
    } else {
      Serial.println("Error opening file");
    }
  }
  digitalWrite(SD_SS_PIN, HIGH);
  digitalWrite(MFRC522_SS_PIN, HIGH);
}