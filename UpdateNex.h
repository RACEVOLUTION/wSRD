float tftNexCurrent;

void execNex() {
  Serial.print("connecting to ");
  Serial.println(hostNex);

  if (!http.begin(String("http://") + hostNex + tftNex)) {
    Serial.println("connection failed");
    myNex.writeStr("c.txt", "Connection failed...");
    return;
  }

  Serial.print("Requesting URL: ");
  Serial.println(tftNex);


  // This will send the (get) request to the server
  int code = http.GET();
  int contentLength = http.getSize();

  // Update the nextion display
  if (code == 200) {
    //Serial.println("File received. Update Nextion...");
    myNex.writeStr("c.txt", "Firmware received.");

    bool result;
    //Serial2.end();
    // initialize ESPNexUpload
    //delay(500);

    ESPNexUpload nextion(921600);

    // set callback: What to do / show during upload..... Optional!
    nextion.setUpdateProgressCallback([]() {
      //Serial.print(".");
    });

    // prepare upload: setup serial connection, send update command and send the expected update size
    result = nextion.prepareUpload(contentLength);

    if (!result) {
      //Serial.println("Error: " + nextion.statusMessage);
    } else {
      //Serial.print(F("Start upload. File size is: "));
      //Serial.print(contentLength);
      //Serial.println(F(" bytes"));

      // Upload the received byte Stream to the nextion
      result = nextion.upload(*http.getStreamPtr());

      if (result) {
        //Serial.println("\nSuccesfully updated Nextion!");
      } else {
        //Serial.println("\nError updating Nextion: " + nextion.statusMessage);
      }
      nextion.end();
      delay(1000);
      ESP.restart();
    }


  } else {
    // else print http error
    //Serial.println(String("HTTP error: ") + http.errorToString(code).c_str());
  }

  http.end();
  //Serial.println("Closing connection\n");
}

void CheckNexFirmware() {
  myNex.writeStr("page start");
  tftNexCurrent = myNex.readStr("main.fw.txt").toFloat();
  DEBUG_SERIAL.println("-------------------------");
  DEBUG_SERIAL.println("Check For New Display Firmware:");
  if (reqNexFirmware > tftNexCurrent) {
    myNex.writeStr("c.txt", "Update Display...");
    DEBUG_SERIAL.println("New Version. Start Updating Display.");
    execNex();
  } else {
    myNex.writeStr("c.txt", "Display Firmware " + String(tftNexCurrent));
    DEBUG_SERIAL.println("No New Version. Display Is Up To Date.");
  }
  DEBUG_SERIAL.println("-------------------------");
}