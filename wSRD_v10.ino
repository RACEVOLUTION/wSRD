#define DEBUG true  //set to true for debug output, false for no debug output
#define DEBUG_SERIAL \
  if (DEBUG) Serial

#define VSPI_MISO 12
#define VSPI_MOSI 14
#define VSPI_SCLK 13
#define VSPI_SS 21
#define LED_GPIO 19
#define LED_COUNT 15

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define VSPI FSPI
#endif

#define DEBUG_PRINT (1)

const float mcuFirmware = 1.51;
const float reqNexFirmware = 1.51;
String updateUrl = "http://update.racevolution.com/wsrde/update.json";
String hostNex = "update.racevolution.com/wsrde";
String tftNex = "/151.tft";

#include <SPIFFS.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <EasyNextionLibrary.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ESPNexUpload.h>
#include <math.h>
#include <CircularBuffer.h>
#include <NeoPixelBus.h>

TaskHandle_t Task1;
TaskHandle_t Task2;

WiFiManager wm;
WiFiUDP udp;
WiFiClient client;
HTTPClient http;
EasyNex myNex(Serial2);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(LED_COUNT, LED_GPIO);
SPIClass *vspi = NULL;

#include "F1Structs2022.h"
#include "Variables.h"
#include "UpdateFW.h"
#include "UpdateNex.h"
#include "Functions.h"

void setup() {

  Serial.begin(115200);
  DEBUG_SERIAL.println("Serial0 interface initialized.");
  myNex.begin(921600);
  DEBUG_SERIAL.println("Nextion Serial2 interface initialized.");
  strip.Begin();
  DEBUG_SERIAL.println("WS2812B initialized.");

  wm.setAPCallback(SetPortalPage);
  
  vspi = new SPIClass(VSPI);
  vspi->begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);  //SCLK, MISO, MOSI, SS
  pinMode(vspi->pinSS(), OUTPUT);                         //VSPI SS
  DEBUG_SERIAL.print("VSPI initialized. ");
  DEBUG_SERIAL.print(" CLK:");
  DEBUG_SERIAL.print(VSPI_SCLK);
  DEBUG_SERIAL.print(" MISO:");
  DEBUG_SERIAL.print(VSPI_MISO);
  DEBUG_SERIAL.print(" MOSI:");
  DEBUG_SERIAL.print(VSPI_MOSI);
  DEBUG_SERIAL.print(" CS:");
  DEBUG_SERIAL.println(VSPI_SS);

  if (!SD.begin(VSPI_SS, *vspi)) {
    DEBUG_SERIAL.println("-------------------------");
    DEBUG_SERIAL.println("SD-Card Mount Failed");
    DEBUG_SERIAL.println("-------------------------");
  } else {
    DEBUG_SERIAL.println("-------------------------");
    DEBUG_SERIAL.println("SD-Card Mount Ok!");
    DEBUG_SERIAL.println("-------------------------");
  }

  LoadSettings();

  WiFi.mode(WIFI_STA);

  bool res;
  res = wm.autoConnect("wSRD", "simracing");
  if (!res) {
    // myNex.writeStr("page start");
    // delay(100);
    // myNex.writeStr("h.txt", "Failed to connect");
    // myNex.writeStr("c.txt", "Could not connect to your wifi.\\rReset the wifi configuration and try again.");
    // myNex.writeStr("vis breset,1");
  } else {
    CheckNexFirmware();
    myNex.writeStr("page main");
  }

  udp.begin(F1_UDP_PORT);

  xTaskCreatePinnedToCore(
    Task1code, /* Function to implement the task */
    "Task1",   /* Name of the task */
    10000,     /* Stack size in words */
    NULL,      /* Task input parameter */
    0,         /* Priority of the task */
    &Task1,    /* Task handle. */
    0);        /* Core where the task should run */

  xTaskCreatePinnedToCore(
    Task2code, /* Function to implement the task */
    "Task2",   /* Name of the task */
    10000,     /* Stack size in words */
    NULL,      /* Task input parameter */
    1,         /* Priority of the task */
    &Task2,    /* Task handle. */
    1);        /* Core where the task should run */
}

void Task1code(void *parameter) {  //core 0
  for (;;) {
    delay(1);
    int packetSize = udp.parsePacket();
    if (packetSize > 0) {
      int packet_length = udp.read(packetBuffer, sizeof(packetBuffer));
      if (packet_length >= sizeof(PacketHeader)) {
        struct PacketHeader *header = (struct PacketHeader *)&packetBuffer;
        HandleSessionUid(header->m_sessionUID);
        if (header->m_packetFormat == 2022) {
          uint8_t myCar = header->m_playerCarIndex;
          switch (header->m_packetId) {

            case 2:  //LAP DATA PACKET
              {
                struct PacketLapData *p;
                p = (struct PacketLapData *)packetBuffer;
                struct LapData *telemetry = &(p->m_lapData[myCar]);
                currentLap = telemetry->m_currentLapNum;


                myNex.writeNum("main.pos.val", telemetry->m_carPosition);
                myNex.writeNum("main.lap.val", telemetry->m_currentLapNum);

                if (telemetry->m_pitLaneTimerActive) {
                  myNex.writeStr("rv.plt.txt", GetTimeFromMillis(telemetry->m_pitLaneTimeInLaneInMS));
                }


                if (lastLapTime != telemetry->m_lastLapTimeInMS) {  //Handle Lap Time
                  lastLapTime = telemetry->m_lastLapTimeInMS;
                  myNex.writeStr("main.llap.txt", GetTimeFromMillis(telemetry->m_lastLapTimeInMS));

                  tenLastLapTime.unshift(laptimes{ currentLap - 1, lastLapTime });

                  using index_t = decltype(tenLastLapTime)::index_t;
                  for (index_t i = 0; i < tenLastLapTime.size(); i++) {
                    String myNexObjectLapTime = "ll.lt" + String(i) + ".txt";
                    String myNexObjectLap = "ll.l" + String(i) + ".txt";
                    myNex.writeStr(myNexObjectLapTime, GetTimeFromMillis(tenLastLapTime[i].laptime));
                    myNex.writeStr(myNexObjectLap, String(tenLastLapTime[i].lap));
                  }

                  if (fastestLapTime == 0.00 || lastLapTime < fastestLapTime) {
                    fastestLapTime = lastLapTime;
                    myNex.writeStr("main.blap.txt", GetTimeFromMillis(fastestLapTime));
                  }
                }

                if (currentPage == 9 || currentPage == 12) {
                  if (checkPointCarInFrontSet == true && checkPointCarInFrontDistance <= telemetry->m_totalDistance)  //Car in front
                  {
                    myNex.writeStr("dd.dcarinfront.txt", GetTimeFromMillis(millis() - checkPointCarInFrontMillis));
                    checkPointCarInFrontSet = false;
                  }

                  if (!checkPointYourCarSet) {
                    checkPointYourCarDistance = telemetry->m_totalDistance;
                    checkPointYourCarMillis = millis();
                    checkPointYourCarSet = true;
                  }

                  for (int i = 0; i < numActiveCars; i++) {
                    struct LapData *opponent = &(p->m_lapData[i]);

                    if (telemetry->m_carPosition == 1) {          //First Position
                      myNex.writeStr("dd.dcarinfront.txt", "");   //Clear to hide car in front on Nextion
                      myNex.writeStr("dd.llcarinfront.txt", "");  //Clear to hide car in front on Nextion
                      myNex.writeStr("dd.blcarinfront.txt", "");  //Clear to hide car in front on Nextion
                      IdxCarInFront = -1;
                    } else if (telemetry->m_carPosition != 1 && opponent->m_carPosition == (telemetry->m_carPosition - 1)) {
                      if (!checkPointCarInFrontSet) {
                        checkPointCarInFrontDistance = opponent->m_totalDistance;
                        checkPointCarInFrontMillis = millis();
                        checkPointCarInFrontSet = true;

                        IdxCarInFront = i;

                        myNex.writeStr("dd.llcarinfront.txt", GetTimeFromMillis(opponent->m_lastLapTimeInMS));  //Set last laptime car in front
                      }
                    }

                    if (telemetry->m_carPosition == numActiveCars)  //Last Position
                    {
                      myNex.writeStr("dd.dcarbehind.txt", "");   //Clear to hide car behind on Nextion
                      myNex.writeStr("dd.llcarbehind.txt", "");  //Clear to hide car behind on Nextion
                      myNex.writeStr("dd.blcarbehind.txt", "");  //Clear to hide car behind on Nextion
                      IdxCarBehind = -1;
                    } else if (opponent->m_carPosition == (telemetry->m_carPosition + 1))  //Car behind
                    {
                      if (checkPointYourCarSet == true && checkPointYourCarDistance <= opponent->m_totalDistance) {
                        myNex.writeStr("dd.dcarbehind.txt", GetTimeFromMillis(millis() - checkPointYourCarMillis));
                        myNex.writeStr("dd.llcarbehind.txt", GetTimeFromMillis(opponent->m_lastLapTimeInMS));  //Set last laptime car behind
                        checkPointYourCarSet = false;
                        IdxCarBehind = i;
                      }
                    }
                  }
                }
              }
              break;

            case 3:
              {  //EVENT PACKET
                if (switchPageByButton) {
                  struct PacketEventData *p;
                  p = (struct PacketEventData *)packetBuffer;
                  union EventDataDetails *e = &(p->m_eventDetails);

                  eventCode = (char *)p->m_eventStringCode;
                  eventCode = eventCode.substring(0, 4);
                  //Serial.println(eventCode);
                  if ((e->Buttons.m_buttonStatus & BUTTON_MASKS[switchPageButton]) && eventCode == "BUTN") {  //e->Buttons.m_buttonStatus <= 559240 &&
                    switch (currentPage) {
                      case 1:
                        {
                          myNex.writeStr("page dd");
                        }
                        break;
                      case 9:
                        {
                          myNex.writeStr("page ld");
                        }
                        break;
                      case 8:
                        {
                          myNex.writeStr("page ty");
                        }
                        break;
                      case 6:
                        {
                          myNex.writeStr("page rv");
                        }
                        break;
                      case 12:
                        {
                          myNex.writeStr("page ll");
                        }
                        break;
                      default:
                        {
                          myNex.writeStr("page main");
                        }
                        break;
                    }
                  } else if (eventCode == "LGOT") {
                    lightsOut = true;
                  } else if (eventCode == "TMPT") {
                    myNex.writeStr("rv.ev.txt", "Team Mate Pits");
                  }
                }
              }
              break;

            case 4:  //PARTICIPANTS PACKET
              {
                if (numActiveCars == 0) {
                  struct PacketParticipantsData *p;
                  p = (struct PacketParticipantsData *)packetBuffer;
                  numActiveCars = p->m_numActiveCars;
                }
              }
              break;

            case 6:  //CAR TELEMETRY PACKET
              {
                struct PacketCarTelemetryData *p;
                p = (struct PacketCarTelemetryData *)packetBuffer;
                struct CarTelemetryData *telemetry = &(p->m_carTelemetryData[myCar]);
                LedRevCounter(telemetry->m_engineRPM);

                if (currentPage == 1 || currentPage == 7 || currentPage == 6 || currentPage == 12) {


                  if (telemetry->m_speed > 220 && telemetry->m_speed > yourTopSpeed) {
                    yourTopSpeed = telemetry->m_speed;
                    myNex.writeStr("rv.ts.txt", String(yourTopSpeed));
                    myNex.writeStr("rv.ev.txt", "New Top Speed");
                  }


                  if (currentPage != 12) {
                    myNex.writeNum("speed.val", telemetry->m_speed);
                    myNex.writeNum("rpm.val", telemetry->m_engineRPM);

                    if (!showSuggestedGear) {
                      myNex.writeStr("gear.txt", GEAR_NAMES[telemetry->m_gear & 0x0F]);
                    } else {
                      myNex.writeStr("gear.txt", String(p->m_suggestedGear));
                    }
                    myNex.writeNum("main.throttle.val", telemetry->m_throttle * 100);
                    myNex.writeNum("main.brake.val", telemetry->m_brake * 100);
                    myNex.writeStr("drs.txt", DRS_NAMES[telemetry->m_drs & 0x0F]);
                    HandleTyreColors(telemetry->m_tyresInnerTemperature);
                  }

                  myNex.writeStr("temprl.txt", String(telemetry->m_tyresInnerTemperature[0]) + "\xB0"
                                                                                               "C");  // RL, RR, FL, FR
                  myNex.writeStr("temprr.txt", String(telemetry->m_tyresInnerTemperature[1]) + "\xB0"
                                                                                               "C");
                  myNex.writeStr("tempfl.txt", String(telemetry->m_tyresInnerTemperature[2]) + "\xB0"
                                                                                               "C");
                  myNex.writeStr("tempfr.txt", String(telemetry->m_tyresInnerTemperature[3]) + "\xB0"
                                                                                               "C");

                  if (currentPage == 12) {
                    HandleTyreColorsText(telemetry->m_tyresInnerTemperature);
                  }
                }
              }
              break;

            case 7:  //CAR STATUS PACKET
              {

                if (lightsOutHandled == false && lightsOut == true) {
                  struct PacketCarStatusData *p;
                  p = (struct PacketCarStatusData *)packetBuffer;
                  struct CarStatusData *telemetry = &(p->m_carStatusData[myCar]);
                  currentFuelCapacity = telemetry->m_fuelInTank;
                  lightsOutHandled = true;
                }

                if (currentPage == 1 || currentPage == 12) {
                  struct PacketCarStatusData *p;
                  p = (struct PacketCarStatusData *)packetBuffer;
                  struct CarStatusData *telemetry = &(p->m_carStatusData[myCar]);

                  if (currentPage != 12) {
                    myNex.writeNum("bias.val", telemetry->m_frontBrakeBias);
                    myNex.writeNum("fuel.val", GetPercent(telemetry->m_fuelCapacity, telemetry->m_fuelInTank));
                    myNex.writeNum("ers.val", GetPercent(4000000.00, telemetry->m_ersStoreEnergy));
                    myNex.writeNum("flag.bco", FLAG_COLORS[telemetry->m_vehicleFiaFlags & 0x0F]);
                    LedFlag(telemetry->m_vehicleFiaFlags & 0x0F);
                    myNex.writeStr("fl.txt", String(telemetry->m_fuelInTank));
                  }

                  if (currentPage == 12) {

                    myNex.writeStr("eu.txt", String(GetPercent(4000000.00, telemetry->m_ersDeployedThisLap)) + "%");
                    myNex.writeStr("eg.txt", String(GetPercent(4000000.00, (telemetry->m_ersHarvestedThisLapMGUK + telemetry->m_ersHarvestedThisLapMGUK))) + "%");
                    if (telemetry->m_drsActivationDistance != 0) {
                      myNex.writeStr("dd.txt", String(telemetry->m_drsActivationDistance) + "m");
                    } else if (telemetry->m_drsAllowed == 1) {
                      myNex.writeStr("dd.txt", "Enabled");
                    } else {
                      myNex.writeStr("dd.txt", "N/A");
                    }
                  }





                  //TODO Calculate average fuel usage per lap
                  if (currentLap > lastFuelLap) {
                    fuelUsedLastLap = telemetry->m_fuelInTank - currentFuelCapacity;
                    myNex.writeStr("fu.txt", String(fuelUsedLastLap) + "kg");
                    currentFuelCapacity = telemetry->m_fuelInTank;
                    lastFuelLap = currentLap;
                  }
                }
              }
              break;

            case 10:  //CAR DAMAGE PACKET
              {
                if (currentPage == 1 || currentPage == 6 || currentPage == 12) {
                  struct PacketCarDamageData *p;
                  p = (struct PacketCarDamageData *)packetBuffer;
                  struct CarDamageData *telemetry = &(p->m_carDamageData[myCar]);
                  myNex.writeStr("twprl.txt", String(int(telemetry->m_tyresWear[0])) + "%");  // RL, RR, FL, FR
                  myNex.writeStr("twprr.txt", String(int(telemetry->m_tyresWear[1])) + "%");
                  myNex.writeStr("twpfl.txt", String(int(telemetry->m_tyresWear[2])) + "%");
                  myNex.writeStr("twpfr.txt", String(int(telemetry->m_tyresWear[3])) + "%");
                  if (currentPage == 12) {
                    HandleTyreWearColorsText(telemetry->m_tyresWear);
                  }
                }
              }
              break;

            case 11:  //SESSION HISTORY PACKET
              {
                if (currentPage == 8 || currentPage == 9) {


                  struct PacketSessionHistoryData *p;
                  p = (struct PacketSessionHistoryData *)packetBuffer;
                  if (p->m_numLaps > 1) {

                    struct LapHistoryData *bestLapTime = &(p->m_lapHistoryData[p->m_bestLapTimeLapNum - 1]);

                    if (myCar == p->m_carIdx) {
                      if (yourBestLap > bestLapTime->m_lapTimeInMS || yourBestLap == 0.00) {
                        yourBestLap = bestLapTime->m_lapTimeInMS;
                        myNex.writeStr("ld.ybestlap.txt", GetTimeFromMillis(bestLapTime->m_lapTimeInMS));
                        myNex.writeStr("ld.ysec2.txt", GetTimeFromMillis(bestLapTime->m_sector2TimeInMS));
                        myNex.writeStr("ld.ysec3.txt", GetTimeFromMillis(bestLapTime->m_sector3TimeInMS));
                        myNex.writeStr("ld.ysec1.txt", GetTimeFromMillis(bestLapTime->m_sector1TimeInMS));
                        myNex.writeNum("ld.ylap.val", p->m_bestLapTimeLapNum);
                      }
                    } else if (IdxCarInFront == p->m_carIdx && currentPage == 9) {
                      myNex.writeStr("dd.blcarinfront.txt", GetTimeFromMillis(bestLapTime->m_lapTimeInMS));
                    } else if (IdxCarBehind == p->m_carIdx && currentPage == 9) {
                      myNex.writeStr("dd.blcarbehind.txt", GetTimeFromMillis(bestLapTime->m_lapTimeInMS));
                    }

                    if (bestLap > bestLapTime->m_lapTimeInMS || bestLap == 0.00) {
                      bestLap = bestLapTime->m_lapTimeInMS;
                      myNex.writeStr("ld.bestlap.txt", GetTimeFromMillis(bestLapTime->m_lapTimeInMS));
                      myNex.writeStr("ld.blsec2.txt", GetTimeFromMillis(bestLapTime->m_sector2TimeInMS));
                      myNex.writeStr("ld.blsec3.txt", GetTimeFromMillis(bestLapTime->m_sector3TimeInMS));
                      myNex.writeStr("ld.blsec1.txt", GetTimeFromMillis(bestLapTime->m_sector1TimeInMS));
                      myNex.writeNum("ld.blap.val", p->m_bestLapTimeLapNum);
                    }

                    struct LapHistoryData *bestSec1 = &(p->m_lapHistoryData[p->m_bestSector1LapNum - 1]);
                    struct LapHistoryData *bestSec2 = &(p->m_lapHistoryData[p->m_bestSector2LapNum - 1]);
                    struct LapHistoryData *bestSec3 = &(p->m_lapHistoryData[p->m_bestSector3LapNum - 1]);

                    if (bsec1 > bestSec1->m_sector1TimeInMS || bsec1 == 0.00) {
                      bsec1 = bestSec1->m_sector1TimeInMS;
                      myNex.writeStr("ld.bsec1.txt", GetTimeFromMillis(bestSec1->m_sector1TimeInMS));
                    }

                    if (bsec2 > bestSec2->m_sector2TimeInMS || bsec2 == 0.00) {
                      bsec2 = bestSec2->m_sector2TimeInMS;
                      myNex.writeStr("ld.bsec2.txt", GetTimeFromMillis(bestSec2->m_sector2TimeInMS));
                    }

                    if (bsec3 > bestSec3->m_sector3TimeInMS || bsec3 == 0.00) {
                      bsec3 = bestSec3->m_sector3TimeInMS;
                      myNex.writeStr("ld.bsec3.txt", GetTimeFromMillis(bestSec3->m_sector3TimeInMS));
                    }
                  }
                }
              }
              break;
            default:
              break;
          }
        }
      }
    }
  }
}


void Task2code(void *parameter) {  //core 1
  for (;;) {
    delay(1);
    myNex.NextionListen();
    if (currentPage != myNex.currentPageId) {
      currentPage = myNex.currentPageId;
      switch (currentPage) {
        case 2:
          myNex.writeStr("s.ip.txt", WiFi.localIP().toString());
          break;
        default:
          break;
      }
    }
  }
}

void loop() {
}
