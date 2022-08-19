
// ======================================  Includes ============================================

/*
 * KaN Gauge
 * https://github.com/Keith-N/KaN-Gauge
 * K. Nason
 * 7/20/2022
 */

/*
 * Arduino Setup:
 * ESP32 - Add to 'Preferences > Additional Boards Manager URL' then install ESP32 from Boards Manager.
 * https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 *
 * Install Libraries
 * u8g2
 * ACAN ESP32
 */

// Libraries
#include <Arduino.h>
#include <U8g2lib.h>    //https://github.com/olikraus/u8g2
#include <ACAN_ESP32.h> //https://github.com/pierremolinaro/acan-esp32
#include <Wire.h>
#include <Preferences.h>

// Other files
#include "DataFunctions.h"
#include "GPIO.h"
#include "GaugeConfig.h"
#include "OTA.h"
#include "Display.h"

CANMessage CANmsg;
Preferences preferences;
TaskHandle_t TASK_CAN;

static void printDataNameAndUnits(sensorData *data)
{
  u8g2.print(data->dataName);
  u8g2.print(" ");
  u8g2.print(data->units);
}

// ======================================  Input Functions  ============================================

/*
 * Select the data displayed for all 4 possible values and LEDs
 *
 */

void configMode()
{
  inConfigMode = 0;
  int i = 0;

  while (inConfigMode == 0)
  {

    // Print current settings
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.setCursor(0, 0);
    u8g2.print("Data # ");
    u8g2.print((i + 1));
    u8g2.setCursor(0, 20);

    switch (i)
    {
    case 0:
      printDataNameAndUnits(ptrData);
      u8g2.setCursor(0, 45);
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.print("<-Next         Type->");
      break;

    case 1:
      printDataNameAndUnits(ptrData2);
      u8g2.setCursor(0, 45);
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.print("<-Next         Type->");
      break;

    case 2:
      printDataNameAndUnits(ptrData3);
      u8g2.setCursor(0, 45);
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.print("<-Next         Type->");
      break;

    case 3:
      printDataNameAndUnits(ptrData4);
      u8g2.setCursor(0, 45);
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.print("<-Next         Type->");
      break;

    case 4:
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.setCursor(0, 0);
      u8g2.print("LEDs for 1x data");
      u8g2.setCursor(0, 20);
      printDataNameAndUnits(ptrDataLed1);
      u8g2.setCursor(0, 45);
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.print("<-Next         Type->");
      break;

    case 5:
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.setCursor(0, 0);
      u8g2.print("LEDs for 2x data");
      u8g2.setCursor(0, 20);
      printDataNameAndUnits(ptrDataLed2);
      u8g2.setCursor(0, 45);
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.print("<-Next         Type->");
      break;

    case 6:
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.setCursor(0, 0);
      u8g2.print("LEDs for 4x data");
      u8g2.setCursor(0, 20);
      printDataNameAndUnits(ptrDataLed4);
      u8g2.setCursor(0, 45);
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.print("<-Next         Type->");
      break;

    case 7:
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.setCursor(0, 0);
      u8g2.print("LED type");
      u8g2.setCursor(0, 20);
      // u8g2.print(ledType);
      // u8g2.print(" ");
      u8g2.print(ledTypeText[ledType]);
      u8g2.setCursor(0, 45);
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.print("<-Next         Type->");
      break;

    default:
      inConfigMode = 1;
      buttonPress = 1;
      setupData();
      saveDataSettings();
      break;
    }

    u8g2.sendBuffer();

    if (buttonPress2 == 1)
    {
      buttonPress2 = 0;
      lastInput2 = (millis());

      switch (i)
      {

      case 7:
        ledType++;
        if (ledType > 6)
        {
          ledType = 0;
        }
        break;

      default:
        dataSet[i]++;
        setupData();
        if (maxSet == 1)
        {
          maxSet = 0;
          dataSet[i] = 0;
        }
      }
    }

    if (buttonPress == 1)
    {
      buttonPress = 0;
      lastInput = (millis());
      i++;
      if (inConfigMode == 1)
        break;
    }
  }
}

/*
 * Setup pointers from the selected data settings for all 4 displayed values and LEDs
 */

void setupData()
{
  ptrData = selectData(dataSet[0]);
  ptrData2 = selectData(dataSet[1]);
  ptrData3 = selectData(dataSet[2]);
  ptrData4 = selectData(dataSet[3]);
  ptrDataLed1 = selectData(dataSet[4]);
  ptrDataLed2 = selectData(dataSet[5]);
  ptrDataLed4 = selectData(dataSet[6]);
}

/*
 * Save the selected data settings and enable them to be restored later
 */
void saveDataSettings()
{
  // open in R/W
  preferences.begin("config", false);
  preferences.putUInt("data0", dataSet[0]);
  preferences.putUInt("data1", dataSet[1]);
  preferences.putUInt("data2", dataSet[2]);
  preferences.putUInt("data3", dataSet[3]);
  preferences.putUInt("dataLED1", dataSet[4]);
  preferences.putUInt("dataLED2", dataSet[5]);
  preferences.putUInt("dataLED4", dataSet[6]);
  preferences.putUInt("ledType", ledType);
  preferences.end();
}

void saveStartup(int s1, int s2)
{
  // open in R/W
  preferences.begin("startup", false);
  preferences.putUInt("logo1", s1);
  preferences.end();
  preferences.begin("startup", false);
  preferences.putUInt("logo2", s2);
  preferences.end();
}

void saveDisplayController(int d)
{
  // open in R/W
  preferences.begin("startup", false);
  preferences.putUInt("display", d);
  preferences.end();
}

/*
 * Verify that startup logos have been set. If not set them to the default value
 */

void checkStartupLogo()
{
  if ((startup == 0) || (startup2 == 0))
  {
    saveStartup(newStartup, newStartup2);
    startup = newStartup;
    startup2 = newStartup2;
  }
}

void checkDisplayController()
{
  if (displayController == 0)
  {
    displayController = newDisplayController;
    saveDisplayController(newDisplayController);
  }
}

void checkForError()
{

  #ifdef RESET_WARNING
    if (lastWarningTime < (millis() - warningResetTime))
    {
      newWarning = false;
    }
  #endif
  if ((warningCounter > lastWarningCount) || (checkEngine > 0))
  {
    newWarning = true;
    lastWarningCount = warningCounter;
    lastWarningTime = millis();
  }
}

/*
 * Indicate that button #1 has been pressed
 * It is debounced to reduce false presses
 */
void buttonISR()
{
  if ((millis() - lastInput) > debounceTimer)
  {
    buttonPress = 1;
  }
}

/*
 * Indicate that button #2 has been pressed
 * It is debounced to reduce false presses
 */
void buttonISR2()
{
  if ((millis() - lastInput2) > debounceTimer)
  {
    buttonPress2 = 1;
  }
}

/*
 * Cycle to the next gauge to display when button 1 is pressed
 */
void nextGauge()
{

  gauge++;

  if (wifiToggled == true)
  {
    disableWifi();
  }

  selectGauge(gauge);
  buttonPress = 0;

  preferences.begin("config", false);
  preferences.putUInt("gauge", gauge);
  preferences.end();

  lastInput = (millis());
}

/*
 * Select the next style of gauge and is dependent on the current gauge
 */
void nextConfig()
{

  buttonPress2 = 0;

  switch (gaugeType)
  {
  case 10:
    wifiToggled = !wifiToggled;
    if (wifiToggled)
    {
      otaSetup();
    }
    else
    {
      disableWifi();
    }
    break;

  case 14:
    inConfigMode = 0;
    break;

  case 15:
    startup++;
    if (startup > 5)
    {
      startup = 0;
    }
    saveStartup(startup, startup2);
    break;

  case 16:
    startup2++;
    if (startup2 > 5)
    {
      startup2 = 0;
    }
    saveStartup(startup, startup2);
    break;

  case 17:
    warningCounter++;
    lastError++;
    break;
    
  default:
    gaugeType++;
    if (gaugeType > numGaugeType)
    {
      gaugeType = 0;
    }
    preferences.begin("config", false);
    preferences.putUInt("gaugeType", gaugeType);
    preferences.end();
    break;
  }

  lastInput2 = millis();
}

/*
 * Return the percentage of the current value of its total range defined by the input min/max values
 */
int getPercent(float current, float minimum, float maximum)
{
  return ((int)100 * (current - minimum) / (maximum - minimum));
}

// ================================= Gauges ===========================================

static void printDataFormatted(sensorData *data)
{
  if (data->useInt)
  {
    u8g2.print((int)data->scaledValue);
  }
  else
  {
    u8g2.print(data->scaledValue);
  }
}

static void printOnOff(const char *label, bool on)
{
  u8g2.print(label);

  if (on)
  {
    u8g2.print("On");
  }
  else
  {
    u8g2.print("Off");
  }
}

static void renderLeds(int ledType, sensorData *data)
{
  int percent = getPercent(data->scaledValue, data->minimum, data->maximum);

  switch (ledType)
  {
  case 0:
    sequentialLedAll(percent);
    break;

  case 1:
    sequentialLed(percent);
    break;

  case 2:
    singleLedAll(percent);
    break;

  case 3:
    singleLed(percent);
    break;

  case 4:
    break;

  case 5:
    sequentialLed(percent);

    if (newWarning == true)
    {
      toggleLeftLed();
    }

    if (data->scaledValue > data->alertHigh || data->scaledValue < data->alertLow)
    {
      toggleRightLed();
    }
    break;

  case 6:
    singleLed(percent);

    if (newWarning == true)
    {
      toggleLeftLed();
    }

    if (data->scaledValue > data->alertHigh || data->scaledValue < data->alertLow)
    {
      toggleRightLed();
    }
    break;

  default:
    sequentialLedAll(percent);
    break;
  }
}

/*
 * Print the selected gauge and data on the display
 */
void printData(int g)
{

  // Offset data printed on 4x gauge
  int h = -7;

  switch (g)
  {

  case 0:
    // 1x Sensor Gauge
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.setCursor(0, 0);
    printDataNameAndUnits(ptrData);

    u8g2.setFont(u8g2_font_fub35_tf);
    u8g2.setCursor(0, 26);

    printDataFormatted(ptrData);

    renderLeds(ledType, ptrDataLed1);

    u8g2.sendBuffer();
    break;

  case 1:
    // 2x Sensor Gauge
    u8g2.clearBuffer();
    u8g2.setCursor(0, 0);
    u8g2.setFont(u8g2_font_helvB24_tf);

    printDataFormatted(ptrData);

    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(105, 0);
    u8g2.print(ptrData->dataName);
    u8g2.setCursor(105, 15);
    u8g2.print(ptrData->units);

    u8g2.setCursor(0, 30);
    u8g2.setFont(u8g2_font_helvB24_tf);

    printDataFormatted(ptrData2);

    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(105, 30);
    u8g2.print(ptrData2->dataName);
    u8g2.setCursor(105, 45);
    u8g2.print(ptrData2->units);

    renderLeds(ledType, ptrDataLed2);

    u8g2.sendBuffer();
    break;

  case 2:
    // 4x Sensor Gauge
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 0);
    printDataNameAndUnits(ptrData);
    u8g2.setCursor((displayWidth / 2), 0);
    printDataNameAndUnits(ptrData2);
    u8g2.setCursor(0, displayHeight / 2);
    printDataNameAndUnits(ptrData3);
    u8g2.setCursor((displayWidth / 2), displayHeight / 2);
    printDataNameAndUnits(ptrData4);

    // Setup sensor values
    u8g2.setFont(u8g2_font_t0_22b_tn);

    u8g2.setCursor(0, (displayHeight / 4) + h);
    printDataFormatted(ptrData);

    u8g2.setCursor((displayWidth / 2), (displayHeight / 4) + h);
    printDataFormatted(ptrData2);
    u8g2.setCursor(0, (displayHeight * 3 / 4) + h);
    printDataFormatted(ptrData3);

    u8g2.setCursor((displayWidth / 2), (displayHeight * 3 / 4) + h);
    printDataFormatted(ptrData4);

    renderLeds(ledType, ptrDataLed4);

    u8g2.sendBuffer();

    break;
    // ----------------- Info / Settings ------------------------------

  case 8:
    // Print ECU error status
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.clearBuffer();
    u8g2.setCursor(0, 0);
    u8g2.print("Warnings: ");
    u8g2.print(warningCounter);
    u8g2.setCursor(0, 15);
    u8g2.print("Last Error:  ");
    u8g2.print(lastError);
    u8g2.setCursor(0, 30);
    ledOff();
    u8g2.sendBuffer();
    break;

  case 9:
    // Print ECU status of rev limit, fuel pump, CEL, o2 heater,
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.clearBuffer();

    u8g2.setCursor(0, 0);
    printOnOff("Rev Limit ", revLimit > 0);

    u8g2.setCursor(0, 15);
    printOnOff("Fuel Pump ", fuelPump > 0);

    u8g2.setCursor(0, 30);
    printOnOff("Check Engine ", checkEngine > 0);

    u8g2.setCursor(0, 45);
    printOnOff("EGO Heater ", egoHeater > 0);

    ledOff();
    u8g2.sendBuffer();
    break;

    // WIFI
  case 10:
    // OTA / WiFi Setup
    ledOff();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.clearBuffer();
    u8g2.setCursor(0, 0);
    u8g2.print("WiFi OTA");

#ifdef OTA_ENABLE
    if (wifiToggled == true)
    {
      u8g2.print(" Enabled");
      u8g2.setCursor(0, 16);
      u8g2.print("SSID: ");
      u8g2.print(ssid);
      u8g2.setCursor(0, 28);
      u8g2.print("Pass: ");
      u8g2.print(password);
      u8g2.setCursor(0, 40);
      u8g2.print("IP: ");
      u8g2.print(WiFi.softAPIP());
      u8g2.setCursor(0, 53);
      u8g2.print("Press -> to disable");
      ota();
    }
    else
    {

      u8g2.print(" Disabled");
      u8g2.setCursor(0, 25);
      u8g2.print("Press -> to enable");
    }
#else
    u8g2.setCursor(0, 15);
    u8g2.print("Feature disabled");
    u8g2.setCursor(0, 30);
    u8g2.print("Update required!");
#endif

    ledOff();
    u8g2.sendBuffer();
    break;

  case 11:
    printGitQR();
    break;

    // INFO
  case 12:
    // Print build, and CAN info
    // Useful for checking CAN connection
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.clearBuffer();
    u8g2.setCursor(0, 0);
    if (rxTimeout == 1)
    {
      u8g2.print("No data recieved");
    }
    else
    {
      u8g2.print("Recieving data");
    }
    u8g2.setCursor(0, 15);
    u8g2.print("Curent Time: ");
    u8g2.print(millis());
    u8g2.setCursor(0, 30);
    u8g2.print("Last recieved: ");
    u8g2.print(lastMessage);
    u8g2.setCursor(0, 45);
    u8g2.print("Buffer: ");
    u8g2.print((int)ACAN_ESP32::can.driverReceiveBufferCount());
    u8g2.setCursor(100, 45);
    u8g2.print(build);
    ledOff();
    u8g2.sendBuffer();
    break;

    // CONFIG MODE
  case 14:
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 0);
    if (inConfigMode == 0)
    {
      configMode();
    }
    else
    {
      u8g2.setCursor(0, 0);
      u8g2.print("Gauge Configuration");
      u8g2.setCursor(0, 25);
      u8g2.print("Press -> to enter");
    }
    u8g2.sendBuffer();
    ledOff();
    break;

    // STARTUP CHANGE
  case 15:
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 0);
    u8g2.print("startup logo");
    u8g2.setCursor(0, 25);
    u8g2.print("Current : ");
    u8g2.print(startup);
    u8g2.sendBuffer();
    break;

  case 16:
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 0);
    u8g2.print("startup logo 2");
    u8g2.setCursor(0, 25);
    u8g2.print("Current : ");
    u8g2.print(startup2);
    u8g2.sendBuffer();
    break;

  case 17:
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 0);
    u8g2.print("Warning Count");
    u8g2.setCursor(0, 25);
    u8g2.print("Current : ");
    u8g2.print(warningCounter);
    u8g2.setCursor(0, 45);
    u8g2.print("Press -> to +1");
    u8g2.sendBuffer();
    break;
    
  // Use gauge type 0 as default
  default:
    ledOff();
    gaugeType = 0;
    gauge = 0;
    reset = 0;
  }
}

/*
 * Returns a pointer to the selected data type to be used
 */

sensorData *selectData(int g)
{

  /*
   * setup pointer to select proper sensor/data from sensorData
      warningStatus
      warningCounter
      rpm
      ignitionTiming
      injectorDuty
      vss
      accelerator
      throttle1
      throttle2
      manifoldPressure
      coolantTemperature
      intakeTemperature
      auxTemp1
      auxTemp2
      mcuTemp
      fuel
      afr
      oilPressure
      vvtPosition
      batteryVoltage
      airMass
      estimatedAirFlow
      injectorPulse
   */

  switch (g)
  {

  case 0:
    return &afr;
    break;

  case 1:
    return &rpm;
    break;

  case 2:
    return &vss;
    break;

  case 3:
    return &vss_mph;
    break;

  case 4:
    return &manifoldPressure;
    break;

  case 5:
    return &manifoldPressure_psi;
    break;

  case 6:
    return &coolantTemperature;
    break;

  case 7:
    return &coolantTemperature_f;
    break;

  case 8:
    return &intakeTemperature;
    break;

  case 9:
    return &intakeTemperature_f;
    break;

  case 10:
    return &accelerator;
    break;

  case 11:
    return &ignitionTiming;
    break;

  case 12:
    return &injectorDuty;
    break;

  case 13:
    return &injectorPulse;
    break;

  case 14:
    return &throttle1;
    break;

  case 15:
    return &throttle2;
    break;

  case 16:
    return &vvtPosition;
    break;

  case 17:
    return &airMass;
    break;

  case 18:
    return &estimatedAirflow;
    break;

  case 19:
    return &fuel;
    break;

  case 20:
    return &mcuTemp;
    break;

  case 21:
    return &auxTemp1;
    break;

  case 22:
    return &auxTemp2;
    break;

  case 23:
    return &batteryVoltage;
    break;

  case 24:
    return &ethanol;
    break;

  case 25:
    return &lambda1;
    break;

  case 26:
    return &lambda2;
    break;

  case 27:
    return &fuelPressureLow;
    break;

  case 28:
    return &fuelPressureHigh;
    break;

  case 29:
    return &fuelConsumed;
    break;

  case 30:
    return &fuelConsumption;
    break;

  case 31:
    return &fuelTrim;
    break;

  case 32:
    return &vvtIntake1;
    break;

  case 33:
    return &vvtIntake2;
    break;

  case 34:
    return &vvtExhaust1;
    break;

  case 35:
    return &vvtExhaust2;
    break;

#ifdef DEBUG_BUILD
  case 36:
    return &testData;
    break;
#endif

  default:
    maxSet = 1;
    return &afr;
  }
}

void selectGauge(int g)
{

  switch (g)
  {

  case 0:
    preferences.begin("config", true);
    gaugeType = preferences.getUInt("gaugeType", 0);
    preferences.end();
    break;

  case 1:
    gaugeType = 9;
    break;

  case 2:
    gaugeType = 8;
    break;

  case 3:
    gaugeType = 10;
    break;

  case 4:
    gaugeType = 14;
    break;

  case 5:
    gaugeType = 11;
    break;

#ifdef DEBUG_BUILD
  case 6:
    gaugeType = 15;
    break;

  case 7:
    gaugeType = 16;
    break;

  case 8:
    gaugeType = 17;
    break;
#endif

  default:
    gauge = 0;
    gaugeType = 0;
  }
}

// -----------------------------------      CAN      ---------------------------------------------

/*
 * This function is used to setup filtering on incoming CAN messages, restricting to only standard frames (11bit)
 */
//
void canSetupFiltered(int filterID, int maskID)
{

  // Setup CAN for desired bit rate, can be adjusted in config
  int DESIRED_BIT_RATE = CAN_RATE_k * 1000UL;
  ACAN_ESP32_Settings settings(DESIRED_BIT_RATE);
  settings.mDriverReceiveBufferSize = CAN_RX_BUFFER_FILTERED;

  // Setup Tx/Rx pins
  settings.mRxPin = CAN_RX;
  settings.mTxPin = CAN_TX;

  // Setup Filter and mask from given values
  const ACAN_ESP32_Filter filter = ACAN_ESP32_Filter::singleStandardFilter(ACAN_ESP32_Filter::data, filterID, maskID);
  // const uint32_t errorCode =
  ACAN_ESP32::can.begin(settings, filter);
}

/*
 * This function is used to initialize CAN, restricting to only standard frames (11bit)
 */
void canSetup()
{

  // Setup CAN for desired bit rate, can be adjusted in config
  int DESIRED_BIT_RATE = CAN_RATE_k * 1000UL;
  ACAN_ESP32_Settings settings(DESIRED_BIT_RATE);
  settings.mDriverReceiveBufferSize = CAN_RX_BUFFER;

  // Setup Tx/Rx pins
  settings.mRxPin = CAN_RX;
  settings.mTxPin = CAN_TX;
  const ACAN_ESP32_Filter filter = ACAN_ESP32_Filter::acceptStandardFrames();
  // const uint32_t errorCode =
  ACAN_ESP32::can.begin(settings, filter);
}

/*
 * This function is used to create a task to assign to core 0, to utilize the second core for processing CAN messages
 */
void canTask(void *pvParameters)
{

  for (;;)
  {

    if (ACAN_ESP32::can.receive(CANmsg))
    {
      SAVE_DATA(CANmsg);
      lastMessage = millis();
      rxTimeout = 0;
    }

// Increment Testing Data if in DEBUG
#ifdef DEBUG_BUILD
    incrementTestData();
#endif
  }
}

// ======================================  MAIN  ============================================

void setup()
{

#ifdef RESET_STORED
  preferences.begin("config", false);
  preferences.clear();
  preferences.end();

  preferences.begin("startup", false);
  preferences.clear();
  preferences.end();
#endif

  // Configure GPIO and interrupt
  PIN_SETUP();
  ledOff();
  attachInterrupt(digitalPinToInterrupt(USER_INPUT), buttonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(USER_INPUT2), buttonISR2, FALLING);

  // Make sure ptr is initialized to something
  ptrData = &afr;
  ptrData2 = &coolantTemperature;
  ptrData3 = &intakeTemperature;
  ptrData4 = &manifoldPressure;
  ptrDataLed1 = &afr;
  ptrDataLed2 = &afr;
  ptrDataLed4 = &afr;

  // use to force a startup image
#ifdef SETUP_STARTUP
  startup = newStartup;
  startup2 = newStartup2;
  saveStartup();
#endif

  // Get the previous gauge values to start from last gauge
  preferences.begin("config", true);
  gauge = preferences.getUInt("gauge", 0);
  dataSet[0] = preferences.getUInt("data0", 0);
  dataSet[1] = preferences.getUInt("data1", 0);
  dataSet[2] = preferences.getUInt("data2", 0);
  dataSet[3] = preferences.getUInt("data3", 0);
  dataSet[4] = preferences.getUInt("dataLED1", 0);
  dataSet[5] = preferences.getUInt("dataLED2", 0);
  dataSet[6] = preferences.getUInt("dataLED4", 0);
  ledType = preferences.getUInt("ledType", 0);
  preferences.end();

  preferences.begin("startup", true);
  startup = preferences.getUInt("logo1", 0);
  startup2 = preferences.getUInt("logo2", 0);
  displayController = preferences.getUInt("display", 0);
  preferences.end();

#ifdef USE_SAVED_DISP_CONTROLLER
  switch (displayController)
  {
  case 1:
    break;

  case 2:
    break;

  default:
    break;
  }

#endif

  checkStartupLogo();

  // Return to data display if at config screens
  if (gauge > 2)
  {
    gauge = 0;
  }
  selectGauge(gauge);

  // Start the OLED display
  u8g2.begin();
  oledSetup();

// Print the splash screen
#ifdef USE_BMP
  switch (startup)
  {
  case 0:
  default:
    printBuild();
    break;

  case 1:
    printBMP_KaN();
    break;

  case 2:
    printBMP_rusEFI();
    break;

  case 3:
    printBMP_BMM();
    break;

  case 4:
    printBMP_GG();
    break;
  }

  // Wait some time for the splash screen to show
  while (millis() < (startTime))
    ;
#endif

#ifdef CAN_FILTERED
  canSetupFiltered(ptrData->canID, 0);
#else
  canSetup();
#endif

  // Use core 0 for processing CAN messages
  xTaskCreatePinnedToCore(
      canTask,    /* Function to implement the task */
      "TASK_CAN", /* Name of the task */
      10000,      /* Stack size in words */
      NULL,       /* Task input parameter */
      0,          /* Priority of the task */
      &TASK_CAN,  /* Task handle. */
      0);         /* Core where the task should run */

  // Sweep the LEDs and wait so that the the startup is seen
  if (ledType == 4)
  {
  }
  else
  {
    ledSweep(0, 12, 40, 20);
  }

  setupData();

#ifdef USE_BMP_2
  switch (startup2)
  {
  case 0:
    break;

  case 1:
    printBMP_KaN();
    while (millis() < (startTime + startTime))
    {
    }
    break;

  case 2:
    printBMP_rusEFI();
    while (millis() < (startTime + startTime))
    {
    }
    break;

  case 3:
    printBMP_BMM();
    while (millis() < (startTime + startTime))
    {
    }
    break;

  case 4:
    printBMP_GG();
    while (millis() < (startTime + startTime))
    {
    }
    break;

  default:
    printBuild();
    while (millis() < (startTime + startTime))
    {
    }
    break;
  }
#endif
}

void loop()
{

  // Slow down display updates to improve legibility
  if ((millis() - lastDisplayUpdate) > displayUpdateRate)
  {
    // Update the display with the currently selected data
    lastDisplayUpdate = millis();
    printData(gaugeType);
  }

  // Check if either button was pressed
  if (buttonPress > 0)
  {
    nextGauge();
  }

  if (buttonPress2 > 0)
  {
    nextConfig();
  }

  checkForError();

  // Alert if communication with ECU is lost
  // Compare current time to last recieved message
#ifdef RX_TIMEOUT
  if ((millis() - lastMessage) > dataTimeout && (rxTimeout == 0))
  {
    rxTimeout = 1;
    gaugeType = 12;
  }
#endif
}
