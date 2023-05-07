# BinnyTheBinTracker
34346 Networking technologies and application development for Internet of Things (IoT) Spring 23 at DTU.

## Webpage for the interaction with the unit
[Binnythebintracker - UI](http://binnythebintracker.com:1880/ui)

## Webpage with the NodeRed code
[Binnythebintracker - NodeRed](http://binnythebintracker.com:1880/)

# Pinout

| Pin (Esp32)   | Function      | Module    |
| ---           | ---           | --- |
| 3.3v          | Voltage inout |  RFID, ultrasonic, Lora, LED's |
| GND           | Ground        |  RFID, ultrasonic, Lora, LED's |
| 15            | ss            |  RFID |
| 12            | Trigger       |  Ultrasonic |
| 13            | LED           |  Lora |
| 14            | Echo          |  Ultrasonic |
| 16            | RX            |  LoRa |
| 17            | TX            |  LoRa |
| 19            | MISO          |  RFID |
| 21            | Reset         |  Lora |
| 32            | Reset         |  RFID |
| 23            | MOSI          |  RFID |
| 27            | LED           |  Green LED |
| 33            | LED           |  Red LED |


## Working With RFID

### Is a UID known?
To create a function that checks if a given UID is in the `knownUIDs` array, you can loop through each row of the array and compare the values in that row with the target UID. If the target UID is found, you can return `true` to indicate that the UID is in the array. If the end of the array is reached without finding a match, you can return `false` to indicate that the UID is not in the array.

Here's an example function that implements this logic:

```
bool IsUIDKnown(byte uid[]) {
  for (int i = 0; i < 200; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (knownUIDs[i][j] != uid[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      // Found a match, UID is known
      return true;
    }
  }
  // UID not found in array
  return false;
}
```

This function takes a byte array `uid` representing the target UID to check. It loops through each row of the `knownUIDs` array using the outer `for` loop, and compares the values in that row with the target UID using the inner `for` loop. If a match is found, the function returns `true` to indicate that the UID is in the array.

If the target UID is not found in the array, the function returns `false` to indicate that the UID is not in the array.

You can call this function whenever you need to check if a UID is in the `knownUIDs` array.



### finding an empty row, and adding a UID to the known UID's
To create a function that finds the first "empty" row in the `knownUIDs` array and returns the corresponding row number, you can loop through each row of the array and check if all the values in that row are equal to 0x00. If you find a row where all the values are 0x00, then that row is considered "empty" and you can return its row number.

Here's an example function that implements this logic:

```
int findEmptyRow() {
  for (int i = 0; i < 200; i++) {
    bool emptyRow = true;
    for (int j = 0; j < 4; j++) {
      if (knownUIDs[i][j] != 0x00) {
        emptyRow = false;
        break;
      }
    }
    if (emptyRow) {
      return i;
    }
  }
  return -1;  // No empty rows found
}
```

This function loops through each row of the `knownUIDs` array using the outer `for` loop, and then checks each value in that row using the inner `for` loop. If all the values in a row are equal to 0x00, the `emptyRow` flag is set to true, and the row number is returned.

If no empty rows are found, the function returns -1 to indicate that the array is full and no more UIDs can be added.

To use this function to add a new UID into the array, you can call the function to get the index of the first empty row, and then copy the new UID values into that row. For example:

```
void addUID(byte uid[]) {
  int emptyRow = findEmptyRow();
  if (emptyRow >= 0) {
    for (int i = 0; i < 4; i++) {
      knownUIDs[emptyRow][i] = uid[i];
    }
    // Optional: print a message to confirm the UID was added
    Serial.println("New UID added to row " + String(emptyRow));
  } else {
    // Array is full, cannot add more UIDs
    Serial.println("Error: array is full");
  }
}
```

This function takes a byte array `uid` representing the new UID to add, and first calls the `findEmptyRow` function to get the index of the first empty row. If an empty row is found, the function copies the values from the `uid` array into the corresponding row in `knownUIDs`.

Note that this function assumes that the `uid` array is a valid byte array with 4 elements. You may want to add additional error checking or input validation if necessary.


### Clearing a UID from knownUIDs
If you just want to empty the row containing a specific UID, so that it can be reused later by an `addUID` function, you can modify the `RemoveUID` function to just clear the contents of the row instead of removing it.

Here's an updated version of the function that clears the row containing a specific UID:

```
void ClearUID(byte uid[]) {
  for (int i = 0; i < 200; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (knownUIDs[i][j] != uid[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      // Found a match, clear the row to indicate it is empty
      for (int l = 0; l < 4; l++) {
        knownUIDs[i][l] = 0x00;
      }
      // Optional: print a message to confirm the UID was cleared
      Serial.println("UID cleared from row " + String(i));
      return;
    }
  }
  // UID not found in array
  Serial.println("Error: UID not found");
}
```

This function works similar to the previous `RemoveUID` function, but instead of overwriting the row with the next row, it just clears the contents of the row by setting all the elements to 0x00.

After clearing the row, the function prints an optional message to confirm that the UID was cleared. If the target UID is not found in the array, the function prints an error message.

You can call this function before calling the `addUID` function to reuse the empty row.

## Conversions

### Float to string

To convert a `float` value to a `String` object in Arduino, you can use the `String()` constructor. Here's an example:

```
float myFloat = 3.14159;
String myString = String(myFloat);
```

In this example, the `float` value `3.14159` is assigned to the variable `myFloat`. The `String()` constructor is then used to create a `String` object from the `myFloat` variable, and assign it to the `myString` variable.

Note that the `String()` constructor has an optional second argument that specifies the number of decimal places to include in the string representation of the `float` value. For example, `String(myFloat, 2)` would create a `String` object with two decimal places, resulting in the string `"3.14"`. If you omit the second argument, the `String` object will include up to six decimal places by default.




## EEPROM

The number of write cycles for the EEPROM on the ESP32 depends on the specific chip used and the manufacturer's specifications. However, in general, EEPROM cells have a limited lifespan and can typically withstand around 100,000 to 1 million write cycles before they start to degrade or fail.

This means that if you are using the EEPROM to store data that is frequently updated or modified, you should be aware of the limitations and design your code accordingly. For example, you can avoid writing to the EEPROM too often, or implement wear-leveling techniques to distribute the writes across multiple cells to prolong the lifespan of the EEPROM.

In general, if you need to store larger amounts of data that will be frequently updated or modified, it may be more appropriate to use a different storage medium, such as an SD card or SPIFFS (SPI Flash File System), which have higher write endurance and are designed for more frequent read/write operations.



The ESP32-WROOM-32D chip used in the DevKit-C development board has 4KB of EEPROM space available for user data storage.

Assuming each UID is stored using 4 bytes, and the array has a maximum size of 200 rows, the total amount of EEPROM space required to store all UIDs would be:

4 bytes per UID x 200 UIDs = 800 bytes

Therefore, 800 bytes of EEPROM space would be required to store all UIDs in the array, which is well within the available 4KB of EEPROM space on the ESP32-WROOM-32D chip.

To calculate the maximum number of possible UIDs that can be stored in the EEPROM, you can use the following equation:

Maximum number of UIDs = EEPROM size / UID size

In this case, the EEPROM size is 4KB (4096 bytes) and the UID size is 4 bytes, so the maximum number of UIDs that can be stored would be:

4096 bytes / 4 bytes per UID = 1024 UIDs

However, it's important to note that the actual number of UIDs that can be stored may be less than the theoretical maximum due to other factors such as the space required for storing metadata or overhead associated with the EEPROM implementation.


### EEPROM Setup

There are several ways to save data on an ESP32, but one common and easy way is to use the EEPROM library, which allows you to store small amounts of data in the non-volatile memory of the ESP32.

Here's an example of how you could use the EEPROM library to store and load the `knownUIDs` array:

```
#include <EEPROM.h>

// Define the size of the EEPROM block to use
#define EEPROM_SIZE 1024

// Define the starting address of the data block
#define DATA_START_ADDRESS 0

// Define the number of UID records to store
#define NUM_UIDS 200

// Define the size of each UID record
#define UID_SIZE 4

// Define the total size of the UID array
#define UID_ARRAY_SIZE (NUM_UIDS * UID_SIZE)

// Declare the UID array
byte knownUIDs[NUM_UIDS][UID_SIZE];

void setup() {
  // Initialize the EEPROM library
  EEPROM.begin(EEPROM_SIZE);

  // Load the UID array from EEPROM
  LoadUIDs();
}

void loop() {
  // Do your main program logic here
  // ...

  // Save the UID array to EEPROM
  SaveUIDs();
}

void SaveUIDs() {
  // Write the UID array to EEPROM
  for (int i = 0; i < NUM_UIDS; i++) {
    for (int j = 0; j < UID_SIZE; j++) {
      EEPROM.write(DATA_START_ADDRESS + i * UID_SIZE + j, knownUIDs[i][j]);
    }
  }

  // Commit the changes to EEPROM
  EEPROM.commit();
}

void LoadUIDs() {
  // Read the UID array from EEPROM
  for (int i = 0; i < NUM_UIDS; i++) {
    for (int j = 0; j < UID_SIZE; j++) {
      knownUIDs[i][j] = EEPROM.read(DATA_START_ADDRESS + i * UID_SIZE + j);
    }
  }
}
```

In this example, we first define several constants to specify the size and layout of the UID data block. We then declare the `knownUIDs` array and define the `SaveUIDs()` and `LoadUIDs()` functions to write and read the UID array to and from EEPROM.

In the `SaveUIDs()` function, we use a nested loop to iterate over each byte in the `knownUIDs` array, and write it to the appropriate address in EEPROM. We then call `EEPROM.commit()` to commit the changes to EEPROM.

In the `LoadUIDs()` function, we use a similar nested loop to read the bytes from EEPROM and store them in the `knownUIDs` array.

In the `setup()` function, we initialize the EEPROM library and call `LoadUIDs()` to load the UID array from EEPROM. In the `loop()` function, we call `SaveUIDs()` to save the UID array to EEPROM after any changes have been made.

Note that the EEPROM library has a limited number of write cycles, so it's important to use it judiciously and avoid writing to it excessively. Also, the ESP32 has other options for non-volatile storage, such as SPIFFS and SD cards, which may be more suitable for larger amounts of data.


