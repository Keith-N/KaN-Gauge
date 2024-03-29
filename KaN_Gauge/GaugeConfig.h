
/*
   Defines
   Use to enable/disable features
*/

String build = "1.2.4";
int nvsVersion = 6;

// Enable first and second startup logos
#define USE_BMP
#define USE_BMP_2

// Select display controller

//#define USE_SAVED_DISP_CONTROLLER
#define FORCED_DISP_CONTROLLER
#define SSD1306
//#define SH1106

// Select boot logos
//#define DEFAULT_START
#define BMM_START
//#define GG_START
//#define DEBUG_BUILD
//#define SERIAL_DEBUG

// Allow OTA updates
#define OTA_ENABLE

//#define WIFI_ALWAYS_ACTIVE

// Override default min/max with saved values
#define CUSTOM_MINMAX

// Alert if too long between CAN messages
//#define RX_TIMEOUT

// Check and reset the stored NVS data on boot
#define RESET_STORED_NVS

// Force the startup logo to newStartup values, writes on boot
//#define SETUP_STARTUP

/*
   0 - Not Set, save new values, shows build version
   1 - KaN
   2 - FOME
   3 - BMM
   4 - GG

   # - Any non defined value shows Build Version

   9 - Update using new startup settings
*/

#ifdef DEFAULT_START
int newStartup = 1;
int newStartup2 = 2;
#elif defined(BMM_START)
int newStartup = 3;
int newStartup2 = 2;
#elif defined(GG_START)
int newStartup = 4;
int newStartup2 = 2;
#else
int newStartup = 0;
int newStartup2 = 0;
#endif

/*
   Variables
   Adjust timers and CAN settings
*/

#ifdef DEBUG_BUILD
String buildType = "Debug";
#elif defined(RESET_STORED)
String buildType = "NVS";
#elif defined(BMM_START)
String buildType = "BMM";
#elif defined(GG_START)
String buildType = "GG";
#else
String buildType = "";
#endif

#ifdef SSD1306
int displayType = 0;
String dispDriver = "SSD1306";
#elif defined(SH1106)
int displayType = 1;
String dispDriver = "SH1106";
#endif

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
int currentFilter = 0;

// Starting filter and mask
// reconfigured based on saved gauge
int filterID = 516;
int maskID = 0;

// NOTE: A larger buffer will hold more data
//       and cause lag between the ECU and gauge
const int CAN_RX_BUFFER = 5;
const int CAN_RX_BUFFER_FILTERED = 1;

// Misc
int gauge = 0;
int gaugeType = 0;
int lastInput = 0;
int lastInput2 = 0;
int buttonPress = 0;
int buttonPress2 = 0;
int lastMessage = 0;
int testMode = 0;
int longPress = 0;
bool wifiToggled = false;
int rxTimeout = 0;

int displayController = 0;
int newDisplayController = 0;

int numGaugeType = 3;

int lastOnTime1 = 0;
int lastOnTime2 = 0;
int lastOffTime1 = 0;
int lastOffTime2 = 0;

int displayWidth = 128;
int displayHeight = 64;
int reset = 0;

int startup = 0;
int startup2 = 0;

int inConfigMode = 1;
int maxSet = 0;

int dataSet[] = {0, 0, 0, 0, 0, 0, 0, 0 , 0 , 0};
int ledType = 0;
String ledTypeText[] = {"All LEDs", "Red LEDs", "Single LED ALL", "Single LED Red", "None", "LEDs with Alert", "Single LED with Alert"};

int lastWarningCount;
bool newWarning;
int lastWarningTime = 0;
int warningResetTime = 5000;

#define RESET_WARNING
int displayUpdateRate = 0;
int displayUpdateTime[] = {0, 50, 75, 100, 150, 200, 250};
//define DISPLAY_UPDATE_RATE
int lastDisplayUpdate = 0;
