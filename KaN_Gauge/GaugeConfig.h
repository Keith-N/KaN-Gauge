
/*
 * Defines
 * Use to enable/disable features
 */
#define SETUP_STARTUP
#define USE_BMP
//#define SHOW_INFO
//#define RX_TIMEOUT
#define OTA_ENABLE
//#define CAN_FILTERED
//#define LED_SINGLE
#define USE_DATATYPE


#define EN_DISP_RES
#define EN_ESP_RES

#define DUAL_CORE_CAN
/*
 * Variables
 * Adjust timers and CAN settings
 */

const String BUILD = "1.0.7_TEST";

// Times in ms
int dataTimeout = 3000;
int infoTime = 1000;
int debounceTimer = 200;
int wifiTimeout = 5000;
int startTime = 1600;

int blinkOn_1 = 250;
int blinOff_1 = 250;


// CAN Settings
const int CAN_BASE_ID = 512;
const int CAN_RATE_k = 500;
int filterList[] = {512, 513};
const int numFilter = 1;
int currentFilter =0;

// Starting filter and mask
// reconfigured based on saved gauge
int filterID = 516;
int maskID = 0;

// NOTE: A larger buffer will hold more data
//       and cause lag between the ECU and gauge
const int CAN_RX_BUFFER = 5;
const int CAN_RX_BUFFER_FILTERED = 1;

// WiFi ota
// Information is retrieved from device flash
const char *host = "gauge_update";
const char *ssid = "gauge_update";
const char *password = "update1234";
String rev;
String wifiStatus = "";

//Misc
int gauge = 0;
int gaugeType = 0;
int lastInput = 0;
int lastInput2 = 0;
int buttonPress = 0;
int buttonPress2 = 0;
int lastMessage = 0;
int testMode = 0;
int longPress = 0;
int wifiToggled = 0;
int percent = 0;
int rxTimeout= 0;

int numGaugeType = 2;

int lastOnTime1 = 0;
int lastOnTime2 = 0;
int lastOffTime1 = 0;
int lastOffTime2 = 0;

int displayWidth = 128;
int displayHeight = 64;
int reset = 0;

/*
 * 0 - Default
 * 1 - BMM
 * 2 -
 * 3 -
 */

int startup = 1;
int newStartup = 1;
int inConfigMode = 1;
int maxSet = 0;

int dataSet[] = {0,0,0,0,0};
