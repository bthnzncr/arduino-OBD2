// Copyright (c) Sandeep Mistry. All rights reserved.
// Copyright (c) 2025 Batuhan Zencir (ESP32 modifications and enhancements)
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <esp32_can.h> // the OBD2 library depends on the CAN library
#include <esp32_obd2.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println(F("OBD2 data printer"));

  while (true) {
    Serial.print(F("Attempting to connect to OBD2 CAN bus ... "));

    if (!OBD2.begin()) {
      Serial.println(F("failed!"));

      delay(1000);
    } else {
      Serial.println(F("success"));
      break;
    }
  }

  Serial.println();
  Serial.print("VIN = ");
  Serial.println(OBD2.vinRead());
  Serial.print("ECU Name = ");
  Serial.println(OBD2.ecuNameRead());

  String dtcCodes = OBD2.dtcRead();
  if (dtcCodes.length() > 0) {
    Serial.print(F("Stored DTCs: "));
    Serial.println(dtcCodes);

    // Example of looking up a DTC code description.

    const char* description = OBD2.getDTCDescription("P0001");
    if (description) {
      Serial.print(F("Description for P0001: "));
      Serial.println(description);
    }
  } else {
    Serial.println(F("No DTCs found."));
  }
}

void loop() {
  // loop through PIDs 0 to 95, reading and printing the values
  for (int pid = 0; pid < 96; pid++) {
    processPid(pid);
  }
  Serial.println();

  // wait 5 seconds before next run
  delay(5000);
}

void processPid(int pid) {
  if (!OBD2.pidSupported(pid)) {
    // PID not supported, continue to next one ...
    return;
  }

  // print PID name
  Serial.print(OBD2.pidName(pid));
  Serial.print(F(" = "));

  if (OBD2.pidValueRaw(pid)) {
    // read the raw PID value
    unsigned long pidRawValue = OBD2.pidReadRaw(pid);

    Serial.print(F("0x"));
    Serial.print(pidRawValue, HEX);
  } else {
    // read the PID value
    float pidValue = OBD2.pidRead(pid);

    if (isnan(pidValue)) {
      Serial.print("error");
    } else {
      // print value with units

      Serial.print(pidValue);
      Serial.print(F(" "));
      Serial.print(OBD2.pidUnits(pid));
    }
  }

  Serial.println();
}

