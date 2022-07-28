
/*
 * Defines
 * Use to enable/disable features
 */


String ver = "1.1.1";
// Enable first and second startup logos
#define USE_BMP
#define USE_BMP_2

// Select boot logos
//#define DEFAULT_START
//#define BMM_START
#define BLANK_START
//#define TESTING_BUILD

// Allow OTA updates
#define OTA_ENABLE

// Alert if too long between CAN messages
//#define RX_TIMEOUT

// Reset the stored NVS data on boot
// #define RESET_STORED
// setup test build defines


#ifdef TESTING_BUILD
  #define CONFIG_STARTUP

  // Force the startup logo to newStartup values, writes on boot
  //#define SETUP_STARTUP

  
#endif

/*
 * 0 - Not Set, save new values, shows build version
 * 1 - KaN
 * 2 - rusEFI
 * 3 - BMM
 * 4 - Empty
 * 
 * # - Any non defined value shows Build Version
 * 
 * 9 - Update using new startup settings
 */

#ifdef DEFAULT_START
  int newStartup = 1;
  int newStartup2 = 2;
#elif defined(BMM_START)
  int newStartup = 3;
  int newStartup2 = 2;
#elif defined(BLANK_START)
  int newStartup = 4;
  int newStartup2 = 4;
#else
  int newStartup = 0;
  int newStartup2 = 0;
#endif



/*
 * Variables
 * Adjust timers and CAN settings
 */

#ifdef TESTING_BUILD
  String BUILD = ver + "*";
#elif defined(RESET_STORED)
  String BUILD = ver + " NVS";
#elif defined(BMM_START)
  String BUILD = ver + " BMM";
#else
  String BUILD = ver;
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
bool wifiToggle = false;
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

int startup = 0;
int startup2 = 0;

int inConfigMode = 1;
int maxSet = 0;

int dataSet[] = {0,0,0,0,0,0,0};
int ledType = 0;
String ledTypeText[] = {"All LEDs", "Red LEDs", "Single LED ALL", "Single LED Red", "None"};
