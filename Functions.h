void HandleSessionUid(uint64_t sessionUid) {  //Reset Lap Timer if theres a new session...
  if (currentSessionUid != sessionUid) {
    currentSessionUid = sessionUid;
    myNex.writeStr("main.llap.txt", "00:00:000");
    myNex.writeStr("main.blap.txt", "00:00:000");
    lastLapTime = 0.00;
    tenLastLapTime.clear();
    fastestLapTime = 0.00;
    myNex.writeNum("ld.ylap.val", 0);
    myNex.writeNum("ld.blap.val", 0);
    myNex.writeStr("ld.ybestlap.txt", "00:00:000");
    myNex.writeStr("ld.ysec1.txt", "00:00:000");
    myNex.writeStr("ld.ysec2.txt", "00:00:000");
    myNex.writeStr("ld.ysec3.txt", "00:00:000");
    myNex.writeStr("ld.bestlap.txt", "00:00:000");
    myNex.writeStr("ld.blsec1.txt", "00:00:000");
    myNex.writeStr("ld.blsec2.txt", "00:00:000");
    myNex.writeStr("ld.blsec3.txt", "00:00:000");
    myNex.writeStr("ld.bsec1.txt", "00:00:000");
    myNex.writeStr("ld.bsec2.txt", "00:00:000");
    myNex.writeStr("ld.bsec3.txt", "00:00:000");

    myNex.writeStr("ll.lt0.txt", "");
    myNex.writeStr("ll.lt1.txt", "");
    myNex.writeStr("ll.lt2.txt", "");
    myNex.writeStr("ll.lt3.txt", "");
    myNex.writeStr("ll.lt4.txt", "");
    myNex.writeStr("ll.lt5.txt", "");
    myNex.writeStr("ll.lt6.txt", "");
    myNex.writeStr("ll.lt7.txt", "");
    myNex.writeStr("ll.lt8.txt", "");
    myNex.writeStr("ll.lt9.txt", "");
    myNex.writeStr("ll.l0.txt", "");
    myNex.writeStr("ll.l1.txt", "");
    myNex.writeStr("ll.l2.txt", "");
    myNex.writeStr("ll.l3.txt", "");
    myNex.writeStr("ll.l4.txt", "");
    myNex.writeStr("ll.l5.txt", "");
    myNex.writeStr("ll.l6.txt", "");
    myNex.writeStr("ll.l7.txt", "");
    myNex.writeStr("ll.l8.txt", "");
    myNex.writeStr("ll.l9.txt", "");

    myNex.writeStr("dd.dcarbehind.txt", "00:00:000");
    myNex.writeStr("dd.dcarinfront.txt", "00:00:000");
    myNex.writeStr("dd.llcarbehind.txt", "00:00:000");
    myNex.writeStr("dd.llcarinfront.txt", "00:00:000");
    myNex.writeStr("dd.blcarbehind.txt", "00:00:000");
    myNex.writeStr("dd.blcarinfront.txt", "00:00:000");

    bsec1 = 0.00;
    bsec2 = 0.00;
    bsec3 = 0.00;
    bestLap = 0.00;
    yourBestLap = 0.00;
    yourTopSpeed = 0;

    currentLap = 0;
    lastFuelLap = 1;
    currentFuelCapacity = 0.0;
    fuelUsedLastLap = 0.0;
    lightsOut = false;
    lightsOutHandled = false;

    checkPointYourCarSet = false;
    checkPointYourCarDistance = 0.00;
    checkPointYourCarMillis = 0;

    checkPointCarInFrontSet = false;
    checkPointCarInFrontDistance = 0.00;
    checkPointCarInFrontMillis = 0;

    eventCode = "";
    numActiveCars = 0;
  }
}

void SetPortalPage(WiFiManager *myWiFiManager) {
  myNex.writeStr("page start");
  delay(100);
  myNex.writeStr("h.txt", "WiFi Configuration");
  myNex.writeStr("c.txt", "Scan the QR code to connect to the device's WiFi.\\rThen open the browser to configure your network.");
  myNex.writeStr("vis barcode,1");
}

void HandleTyreColors(uint8_t values[4]) {  // tc0(rl) tc1(rr) tc2(fl) tc3(fr)
  for (int i = 0; i <= 3; i++) {
    String myNexObject = "main.tc" + String(i) + ".picc";
    if (values[i] < 75) {
      myNex.writeNum(myNexObject, 0);
    } else if (values[i] < 85) {
      myNex.writeNum(myNexObject, 4);
    } else if (values[i] < 103) {
      myNex.writeNum(myNexObject, 3);
    } else if (values[i] < 110) {
      myNex.writeNum(myNexObject, 2);
    } else {
      myNex.writeNum(myNexObject, 1);
    }
  }
}

void HandleTyreColorsText(uint8_t values[4]) {  // tc0(rl) tc1(rr) tc2(fl) tc3(fr)
  for (int i = 0; i <= 3; i++) {
    int color = 65535;
    if (values[i] < 80) {
      color = 65535;
    } else if (values[i] < 90) {
      color = 2047;
    } else if (values[i] < 98) {
      color = 2016;
    } else if (values[i] < 103) {
      color = 64704;
    } else {
      color = 63488;
    }

    switch (i) {
      case 0:
        {  //RL
          myNex.writeNum("temprl.pco", color);
        }
        break;
      case 1:
        {  //RR
          myNex.writeNum("temprr.pco", color);
        }
        break;
      case 2:
        {  //FL
          myNex.writeNum("tempfl.pco", color);
        }
        break;
      case 3:
        {  //FR
          myNex.writeNum("tempfr.pco", color);
        }
        break;
    }
  }
}

void HandleTyreWearColorsText(float values[4]) {  // tc0(rl) tc1(rr) tc2(fl) tc3(fr)
  for (int i = 0; i <= 3; i++) {
    int color = 65535;
    if (values[i] < 20) {
      color = 2016;
    } else if (values[i] < 40) {
      color = 65504;
    } else if (values[i] < 60) {
      color = 64704;
    } else {
      color = 63488;
    }

    switch (i) {
      case 0:
        {  //RL
          myNex.writeNum("twprl.pco", color);
        }
        break;
      case 1:
        {  //RR
          myNex.writeNum("twprr.pco", color);
        }
        break;
      case 2:
        {  //FL
          myNex.writeNum("twpfl.pco", color);
        }
        break;
      case 3:
        {  //FR
          myNex.writeNum("twpfr.pco", color);
        }
        break;
    }
  }
}

int GetPercent(float fuelCapacity, float fuelInTank) {  // ToDo Edit
  return round((fuelInTank / fuelCapacity) * 100.0);
}

String convertMillis(long value) {
  String returnValue;
  if (value < 10) {
    returnValue = "00" + String(value);
  } else if (value < 100) {
    returnValue = "0" + String(value);
  } else {
    returnValue = value;
  }
  return returnValue;
}

String convertMinSec(long value) {
  String returnValue;
  if (value < 10) {
    returnValue = "0" + String(value);
  } else {
    returnValue = value;
  }
  return returnValue;
}

String GetTimeFromMillis(long value) {
  unsigned long milliSeconds;
  unsigned long seconds;
  unsigned long minutes;
  String returnValue;

  milliSeconds = value;
  milliSeconds %= 1000;
  seconds = value / 1000;
  minutes = seconds / 60;
  seconds %= 60;
  returnValue = convertMinSec(minutes) + ":" + convertMinSec(seconds) + ":" + convertMillis(milliSeconds);
  return returnValue;
}

void SyncDisplay() {
  myNex.writeNum("sb.rvar.val", switchPageButton);
  myNex.writeNum("sb.bvar.val", switchPageByButton);
  myNex.writeNum("sg.gfrvar.val", gearFlashRPM);
  myNex.writeNum("sg.tslvar.val", throttleLowerLimit);
  myNex.writeNum("sg.tsuvar.val", throttleUpperLimit);
  myNex.writeNum("sg.rcbvar.val", revCounterBrightness);
  myNex.writeStr("s.mfw.txt", String(mcuFirmware));

  DEBUG_SERIAL.println("-------------------------");
  DEBUG_SERIAL.println("Sync Settings To Display.");
  DEBUG_SERIAL.println("-------------------------");
}

void LedRevCounterFlash() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 100) {

    previousMillis = currentMillis;

    if (ledState == HIGH) {
      ledState = LOW;
      strip.SetPixelColor(3, RgbColor(0, 0, 0));
      strip.SetPixelColor(4, RgbColor(0, 0, 0));
      strip.SetPixelColor(5, RgbColor(0, 0, 0));
      strip.SetPixelColor(6, RgbColor(0, 0, 0));
      strip.SetPixelColor(7, RgbColor(0, 0, 0));
      strip.SetPixelColor(8, RgbColor(0, 0, 0));
      strip.SetPixelColor(9, RgbColor(0, 0, 0));
      strip.SetPixelColor(10, RgbColor(0, 0, 0));
      strip.SetPixelColor(11, RgbColor(0, 0, 0));
    } else {
      ledState = HIGH;
      strip.SetPixelColor(3, RgbColor(0, 0, revCounterBrightness));
      strip.SetPixelColor(4, RgbColor(0, 0, revCounterBrightness));
      strip.SetPixelColor(5, RgbColor(0, 0, revCounterBrightness));
      strip.SetPixelColor(6, RgbColor(0, revCounterBrightness, 0));
      strip.SetPixelColor(7, RgbColor(0, revCounterBrightness, 0));
      strip.SetPixelColor(8, RgbColor(0, revCounterBrightness, 0));
      strip.SetPixelColor(9, RgbColor(revCounterBrightness, 0, 0));
      strip.SetPixelColor(10, RgbColor(revCounterBrightness, 0, 0));
      strip.SetPixelColor(11, RgbColor(revCounterBrightness, 0, 0));
    }
  }
}

void LedRevCounter(uint16_t rpm) {
  if (rpm >= gearFlashRPM) {
    LedRevCounterFlash();
  } else if (rpm >= gearFlashRPM - 200) {
    strip.SetPixelColor(3, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(4, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(5, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(6, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(7, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(8, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(9, RgbColor(revCounterBrightness, 0, 0));
    strip.SetPixelColor(10, RgbColor(revCounterBrightness, 0, 0));
    strip.SetPixelColor(11, RgbColor(0, 0, 0));
  } else if (rpm >= gearFlashRPM - 400) {
    strip.SetPixelColor(3, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(4, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(5, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(6, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(7, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(8, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(9, RgbColor(revCounterBrightness, 0, 0));
    strip.SetPixelColor(10, RgbColor(0, 0, 0));
    strip.SetPixelColor(11, RgbColor(0, 0, 0));
  } else if (rpm >= gearFlashRPM - 600) {
    strip.SetPixelColor(3, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(4, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(5, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(6, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(7, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(8, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(9, RgbColor(0, 0, 0));
    strip.SetPixelColor(10, RgbColor(0, 0, 0));
    strip.SetPixelColor(11, RgbColor(0, 0, 0));
  } else if (rpm >= gearFlashRPM - 800) {
    strip.SetPixelColor(3, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(4, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(5, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(6, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(7, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(8, RgbColor(0, 0, 0));
    strip.SetPixelColor(9, RgbColor(0, 0, 0));
    strip.SetPixelColor(10, RgbColor(0, 0, 0));
    strip.SetPixelColor(11, RgbColor(0, 0, 0));
  } else if (rpm >= gearFlashRPM - 1000) {
    strip.SetPixelColor(3, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(4, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(5, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(6, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(7, RgbColor(0, 0, 0));
    strip.SetPixelColor(8, RgbColor(0, 0, 0));
    strip.SetPixelColor(9, RgbColor(0, 0, 0));
    strip.SetPixelColor(10, RgbColor(0, 0, 0));
    strip.SetPixelColor(11, RgbColor(0, 0, 0));
  } else if (rpm >= gearFlashRPM - 1200) {
    strip.SetPixelColor(3, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(4, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(5, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(6, RgbColor(0, 0, 0));
    strip.SetPixelColor(7, RgbColor(0, 0, 0));
    strip.SetPixelColor(8, RgbColor(0, 0, 0));
    strip.SetPixelColor(9, RgbColor(0, 0, 0));
    strip.SetPixelColor(10, RgbColor(0, 0, 0));
    strip.SetPixelColor(11, RgbColor(0, 0, 0));
  } else if (rpm >= gearFlashRPM - 1400) {
    strip.SetPixelColor(3, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(4, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(5, RgbColor(0, 0, 0));
    strip.SetPixelColor(6, RgbColor(0, 0, 0));
    strip.SetPixelColor(7, RgbColor(0, 0, 0));
    strip.SetPixelColor(8, RgbColor(0, 0, 0));
    strip.SetPixelColor(9, RgbColor(0, 0, 0));
    strip.SetPixelColor(10, RgbColor(0, 0, 0));
    strip.SetPixelColor(11, RgbColor(0, 0, 0));
  } else if (rpm >= gearFlashRPM - 1600) {
    strip.SetPixelColor(3, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(4, RgbColor(0, 0, 0));
    strip.SetPixelColor(5, RgbColor(0, 0, 0));
    strip.SetPixelColor(6, RgbColor(0, 0, 0));
    strip.SetPixelColor(7, RgbColor(0, 0, 0));
    strip.SetPixelColor(8, RgbColor(0, 0, 0));
    strip.SetPixelColor(9, RgbColor(0, 0, 0));
    strip.SetPixelColor(10, RgbColor(0, 0, 0));
    strip.SetPixelColor(11, RgbColor(0, 0, 0));
  } else {
    strip.SetPixelColor(3, RgbColor(0, 0, 0));
    strip.SetPixelColor(4, RgbColor(0, 0, 0));
    strip.SetPixelColor(5, RgbColor(0, 0, 0));
    strip.SetPixelColor(6, RgbColor(0, 0, 0));
    strip.SetPixelColor(7, RgbColor(0, 0, 0));
    strip.SetPixelColor(8, RgbColor(0, 0, 0));
    strip.SetPixelColor(9, RgbColor(0, 0, 0));
    strip.SetPixelColor(10, RgbColor(0, 0, 0));
    strip.SetPixelColor(11, RgbColor(0, 0, 0));
  }
  strip.Show();
}

void LedFlag(int flagStatus) {  //-1 = invalid/unknown, 0 = none, 1 = green 2 = blue, 3 = yellow, 4 = red
  if (flagStatus == -1) {       //unknown
    strip.SetPixelColor(0, RgbColor(0, 0, 0));
    strip.SetPixelColor(1, RgbColor(0, 0, 0));
    strip.SetPixelColor(2, RgbColor(0, 0, 0));
    strip.SetPixelColor(12, RgbColor(0, 0, 0));
    strip.SetPixelColor(13, RgbColor(0, 0, 0));
    strip.SetPixelColor(14, RgbColor(0, 0, 0));
  } else if (flagStatus == 0) {  //none
    strip.SetPixelColor(0, RgbColor(0, 0, 0));
    strip.SetPixelColor(1, RgbColor(0, 0, 0));
    strip.SetPixelColor(2, RgbColor(0, 0, 0));
    strip.SetPixelColor(12, RgbColor(0, 0, 0));
    strip.SetPixelColor(13, RgbColor(0, 0, 0));
    strip.SetPixelColor(14, RgbColor(0, 0, 0));
  } else if (flagStatus == 1) {  // green
    strip.SetPixelColor(0, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(1, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(2, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(12, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(13, RgbColor(0, revCounterBrightness, 0));
    strip.SetPixelColor(14, RgbColor(0, revCounterBrightness, 0));
  } else if (flagStatus == 2) {  //blue
    strip.SetPixelColor(0, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(1, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(2, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(12, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(13, RgbColor(0, 0, revCounterBrightness));
    strip.SetPixelColor(14, RgbColor(0, 0, revCounterBrightness));
  } else if (flagStatus == 3) {  //yellow
    strip.SetPixelColor(0, RgbColor(revCounterBrightness, revCounterBrightness, 0));
    strip.SetPixelColor(1, RgbColor(revCounterBrightness, revCounterBrightness, 0));
    strip.SetPixelColor(2, RgbColor(revCounterBrightness, revCounterBrightness, 0));
    strip.SetPixelColor(12, RgbColor(revCounterBrightness, revCounterBrightness, 0));
    strip.SetPixelColor(13, RgbColor(revCounterBrightness, revCounterBrightness, 0));
    strip.SetPixelColor(14, RgbColor(revCounterBrightness, revCounterBrightness, 0));
  } else if (flagStatus == 4) {  //red
    strip.SetPixelColor(0, RgbColor(revCounterBrightness, 0, 0));
    strip.SetPixelColor(1, RgbColor(revCounterBrightness, 0, 0));
    strip.SetPixelColor(2, RgbColor(revCounterBrightness, 0, 0));
    strip.SetPixelColor(12, RgbColor(revCounterBrightness, 0, 0));
    strip.SetPixelColor(13, RgbColor(revCounterBrightness, 0, 0));
    strip.SetPixelColor(14, RgbColor(revCounterBrightness, 0, 0));
  }
}

void SaveSettings() {
  SD.remove(confFile);

  File file = SD.open(confFile, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to create file");
  }

  StaticJsonDocument<512> doc;
  doc["switchPageButton"] = switchPageButton;
  doc["switchPageByButton"] = switchPageByButton;
  doc["gearFlashRPM"] = gearFlashRPM;
  doc["throttleLowerLimit"] = throttleLowerLimit;
  doc["throttleUpperLimit"] = throttleUpperLimit;
  doc["revCounterBrightness"] = revCounterBrightness;
  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write to file");
  }

  file.close();

  DEBUG_SERIAL.println("-------------------------");
  DEBUG_SERIAL.println("Settings loaded.");
  DEBUG_SERIAL.print("Switch Page Button:");
  DEBUG_SERIAL.println(switchPageButton);
  DEBUG_SERIAL.print("Switch Page By Button:");
  DEBUG_SERIAL.println(switchPageByButton);
  DEBUG_SERIAL.print("Gear Flash RPM:");
  DEBUG_SERIAL.println(gearFlashRPM);
  DEBUG_SERIAL.print("Throttle Lower Limit:");
  DEBUG_SERIAL.println(throttleLowerLimit);
  DEBUG_SERIAL.print("Throttle Upper Limit:");
  DEBUG_SERIAL.println(throttleUpperLimit);
  DEBUG_SERIAL.print("Rev Counter Brightness:");
  DEBUG_SERIAL.println(revCounterBrightness);
  DEBUG_SERIAL.println("-------------------------");

  SyncDisplay();
}

void LoadSettings() {
  if (!SD.exists(confFile)) {

    DEBUG_SERIAL.println("-------------------------");
    DEBUG_SERIAL.println("Config file does not exist.");
    DEBUG_SERIAL.println("Create new config file.");
    DEBUG_SERIAL.println("-------------------------");

    switchPageButton = 3;
    switchPageByButton = true;
    gearFlashRPM = 11500;
    throttleLowerLimit = 20;
    throttleUpperLimit = 30;
    revCounterBrightness = 8;

    SaveSettings();
  } else {
    File file = SD.open(confFile);
    if (file) {
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, file);
      switchPageButton = doc["switchPageButton"];
      switchPageByButton = doc["switchPageByButton"];
      gearFlashRPM = doc["gearFlashRPM"];
      throttleLowerLimit = doc["throttleLowerLimit"];
      throttleUpperLimit = doc["throttleUpperLimit"];
      revCounterBrightness = doc["revCounterBrightness"];
    }

    file.close();
  }


  DEBUG_SERIAL.println("-------------------------");
  DEBUG_SERIAL.println("Settings loaded.");
  DEBUG_SERIAL.print("Switch Page Button:");
  DEBUG_SERIAL.println(switchPageButton);
  DEBUG_SERIAL.print("Switch Page By Button:");
  DEBUG_SERIAL.println(switchPageByButton);
  DEBUG_SERIAL.print("Gear Flash RPM:");
  DEBUG_SERIAL.println(gearFlashRPM);
  DEBUG_SERIAL.print("Throttle Lower Limit:");
  DEBUG_SERIAL.println(throttleLowerLimit);
  DEBUG_SERIAL.print("Throttle Upper Limit:");
  DEBUG_SERIAL.println(throttleUpperLimit);
  DEBUG_SERIAL.print("Rev Counter Brightness:");
  DEBUG_SERIAL.println(revCounterBrightness);
  DEBUG_SERIAL.println("-------------------------");

  SyncDisplay();
}

void trigger0() {  //WiFi Reset
  wm.resetSettings();
  delay(100);
  ESP.restart();
}

void trigger1() {  //Send IP to settings page
  myNex.writeStr("s.ip.txt", WiFi.localIP().toString());
}

void trigger2() {  //Switch to display suggested gear...
  if (showSuggestedGear) {
    showSuggestedGear = false;
  } else {
    showSuggestedGear = true;
  }
}

void trigger3() {
  uint32_t temprvar;
  uint32_t tempbvar;

  temprvar = myNex.readNumber("sb.rvar.val");
  tempbvar = myNex.readNumber("sb.bvar.val");

  if (temprvar != 777777 && tempbvar != 777777) {
    switchPageButton = temprvar;
    switchPageByButton = tempbvar;
    SaveSettings();
    myNex.writeStr("page s");
  }
}

void trigger4() {
  uint32_t tempgfrvar;
  uint8_t temptslvar;
  uint8_t temptsuvar;
  uint8_t temprcbvar;
  tempgfrvar = myNex.readNumber("sg.gfrvar.val");
  temptslvar = myNex.readNumber("sg.tslvar.val");
  temptsuvar = myNex.readNumber("sg.tsuvar.val");
  temprcbvar = myNex.readNumber("sg.rcbvar.val");

  if (tempgfrvar != 777777 && temptslvar != 777777 && temptsuvar != 777777) {
    gearFlashRPM = tempgfrvar;
    throttleLowerLimit = temptslvar;
    throttleUpperLimit = temptsuvar;
    revCounterBrightness = temprcbvar;
    SaveSettings();
    myNex.writeStr("page s");
  }
}

void trigger5() {  //Update Trigger
  execOTA();
}

void trigger6() {  //Update Trigger
  getVersion();
}