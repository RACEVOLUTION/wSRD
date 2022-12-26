const char * confFile = "/config.json";
const uint16_t F1_UDP_PORT = 20777;

const char * const GEAR_NAMES[16] = {
  "N",  // Neutral
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "10",
  "11",
  "12",
  "13",
  "14",
  "R",  // Reverse
};

const char * const DRS_NAMES[16] = { // 0 = not allowed, 1 = allowed, -1 = unknown
  "Off",
  "On",
};

const int FLAG_COLORS[16] = { // -1 = invalid/unknown, 0 = none, 1 = green 2 = blue, 3 = yellow, 4 = red
  0,
  2016,
  1055,
  65504,
  63488,
  65535,
};

const uint32_t BUTTON_MASKS[16] = {
  0x00100000ul, //UPD Action 1
  0x00200000ul, //UPD Action 2
  0x00400000ul, //UPD Action 3
  0x00800000ul, //UDP Action 4
  0x01000000ul, //UDP Action 5
  0x02000000ul, //UDP Action 6
  0x04000000ul, //UDP Action 7
  0x08000000ul, //UDP Action 8
};

struct laptimes {
  uint8_t lap;
  float laptime;
};

uint8_t packetBuffer[4096];
float lastLapTime;
CircularBuffer<laptimes,10> tenLastLapTime;
float fastestLapTime;
float bsec1;
float bsec2;
float bsec3;
float bestLap;
float yourBestLap;
uint16_t yourTopSpeed = 0;
uint64_t currentSessionUid;
uint8_t currentPage = 255;
bool showSuggestedGear = false;
uint8_t currentLap = 0;
uint8_t lastFuelLap = 1;
float currentFuelCapacity = 0.0;
float fuelUsedLastLap = 0.0;
bool lightsOut = false;
bool lightsOutHandled = false;

bool checkPointYourCarSet = false;
float checkPointYourCarDistance;
long checkPointYourCarMillis;

bool checkPointCarInFrontSet = false;
float checkPointCarInFrontDistance;
long checkPointCarInFrontMillis;

int IdxCarInFront = -1;
int IdxCarBehind = -1;

uint8_t numActiveCars;
String eventCode;

int ledState = HIGH;               // TODO Switch to Variabled.h
unsigned long previousMillis = 0;  // TODO Switch to Variabled.h

//-----------------------Settings
uint8_t switchPageButton;
bool switchPageByButton;
uint16_t gearFlashRPM;
uint8_t throttleLowerLimit;
uint8_t throttleUpperLimit;
uint8_t revCounterBrightness;