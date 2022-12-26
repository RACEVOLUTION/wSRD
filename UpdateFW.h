#include <Update.h>

long contentLength = 0;
bool isValidContentType = false;
String host;
String bin;
int port = 80;

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

// OTA Logic
void execOTA() {
  DEBUG_SERIAL.println("Connecting to: " + String(host));
  // Connect to host
  if (client.connect(host.c_str(), port)) {
    // Connection Succeed.
    // Fecthing the bin
    //Serial.println("Download: " + String(bin));
    myNex.writeStr("u.st.txt", "Download: " + String(bin));
    // Get the contents of the bin file
    client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Check what is being sent
    //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
    //                 "Host: " + host + "\r\n" +
    //                 "Cache-Control: no-cache\r\n" +
    //                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        myNex.writeStr("u.st.txt", "Client timeout...");
        //Serial.println("client Timeout !");
        client.stop();
        return;
      }
    }
    // Once the response is available,
    // check stuff

    while (client.available()) {
      // read line till /n
      String line = client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty,
      // this is end of headers
      // break the while and feed the
      // remaining `client` to the
      // Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          //Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        //Serial.println("Got " + String(contentLength) + " bytes from server");
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        //Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    // Connect failed
    // May be try?
    // Probably a choppy network?
    //Serial.println("Connection to " + String(host) + " failed. Please check your setup");
    myNex.writeStr("u.st.txt", "Connection failed...");
    // retry??
    // execOTA();
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);

    // If yes, begin
    if (canBegin) {
      myNex.writeStr("u.st.txt", "Update in progress");
      // No activity would appear on the Serial monitor
      // So be patient. This may take 2 - 5mins to complete
      size_t written = Update.writeStream(client);

      if (written == contentLength) {
        //Serial.println("Written : " + String(written) + " successfully");
      } else {
        //Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
        // retry??
        // execOTA();
      }

      if (Update.end()) {
        //Serial.println("OTA done!");
        if (Update.isFinished()) {
          //Serial.println("Update successfully completed. Rebooting.");
          myNex.writeStr("u.st.txt", "Completed. Rebooting.");
          ESP.restart();
        } else {
          //Serial.println("Update not finished? Something went wrong!");
          myNex.writeStr("u.st.txt", "Error...");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      // not enough space to begin OTA
      // Understand the partitions and
      // space availability
      Serial.println("Not enough space to begin OTA");
      client.flush();
    }
  } else {
    Serial.println("There was no content in the response");
    client.flush();
  }
}

void getVersion() {
  http.begin(client, updateUrl);
  http.GET();
  DynamicJsonDocument doc(256);
  deserializeJson(doc, http.getStream());
  http.end();
  if (doc["mcuFirmware"].as<float>() > mcuFirmware) {
    myNex.writeStr("u.st.txt", "New firmware available!");
    myNex.writeStr("u.nfw.txt", String(doc["mcuFirmware"].as<float>()));
    host = doc["urlFirmware"].as<String>();
    bin = doc["binFirmware"].as<String>();
    client.flush();
  } else {
    myNex.writeStr("u.st.txt", "No new firmware available!");
    myNex.writeStr("u.nfw.txt", String(mcuFirmware));
  }
}