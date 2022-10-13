// ======================================  Includes ============================================

/*
   KaN Gauge
   https://github.com/Keith-N/KaN-Gauge
   K. Nason
   7/20/2022
*/

/*
   Arduino Setup:
   ESP32 - Add to 'Preferences > Additional Boards Manager URL' then install ESP32 from Boards Manager.
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

   Install Libraries
   u8g2
   ACAN ESP32
*/

// Libraries
#include <Arduino.h>
#include <U8g2lib.h>    //https://github.com/olikraus/u8g2
#include <ACAN_ESP32.h> //https://github.com/pierremolinaro/acan-esp32
#include <Wire.h>
#include <Preferences.h>
#include <nvs_flash.h>

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

void disableWifi()
{
#ifdef WIFI_ALWAYS_ACTIVE

#else
  WiFi.softAPdisconnect();
  delay(100);
  WiFi.mode(WIFI_OFF);
  delay(100);
  wifiToggled = false;
#endif
}
// ======================================  Input Functions  ============================================

/*
   Select the data displayed for all 4 possible values and LEDs

*/

void eraseNVS()
{

  nvs_flash_deinit();
  nvs_flash_erase(); // erase the NVS partition and...
  nvs_flash_init();  // initialize the NVS partition.
}

void checkNVS()
{

  //   Check NVS config version and wipe if it is not as expected
  //   Save startup config
  //   This will allow cleaning up stored configs if changes are made to the format

  preferences.begin("startup", true);
  int nvsBuf = preferences.getUInt("nvs", 0);
  preferences.end();

  if ( nvsBuf != nvsVersion) {
    preferences.begin("startup", true);
    startup = preferences.getUInt("logo1", 0);
    startup2 = preferences.getUInt("logo2", 0);
    displayController = preferences.getUInt("display", 0);
    preferences.end();

    eraseNVS();

    preferences.begin("startup", false);
    preferences.putUInt("logo1", startup);
    preferences.putUInt("logo2", startup2);
    preferences.putUInt("nvs", nvsVersion);
    preferences.end();
  }
}

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
    switch (i)
    {
      case 0:
        u8g2.print("1x Gauge Data");
        u8g2.setCursor(0, 20);
        printDataNameAndUnits(ptrData);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 1:
        u8g2.print("2x Gauge Data");
        u8g2.setCursor(0, 20);
        printDataNameAndUnits(ptrData2);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 2:
        u8g2.print("4x Gauge Data 3");
        u8g2.setCursor(0, 20);
        printDataNameAndUnits(ptrData3);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 3:
        u8g2.print("4x Gauge Data 4");
        u8g2.setCursor(0, 20);
        printDataNameAndUnits(ptrData4);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 7:
        u8g2.print("Alt 1x Gauge Data");
        u8g2.setCursor(0, 20);
        printDataNameAndUnits(ptrData5);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 4:
        u8g2.print("LEDs for 1x data");
        u8g2.setCursor(0, 20);
        printDataNameAndUnits(ptrDataLed1);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 5:
        u8g2.print("LEDs for 2x data");
        u8g2.setCursor(0, 20);
        printDataNameAndUnits(ptrDataLed2);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 6:
        u8g2.print("LEDs for 4x data");
        u8g2.setCursor(0, 20);
        printDataNameAndUnits(ptrDataLed4);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 8:
        u8g2.print("LEDs for Alt 1x");
        u8g2.setCursor(0, 20);
        printDataNameAndUnits(ptrDataLed5);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 9:
        u8g2.print("LED type");
        u8g2.setCursor(0, 20);
        u8g2.print(ledTypeText[ledType]);
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         Type->");
        break;

      case 10:
        u8g2.print("Display Update Rate");
        u8g2.setCursor(0, 20);
        u8g2.print(displayUpdateTime[displayUpdateRate]);
        u8g2.print(" ");
        u8g2.print("ms");
        u8g2.setCursor(0, 45);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.print("<-Next         + ->");
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

        case 9:
          ledType++;
          if (ledType > 6)
          {
            ledType = 0;
          }
          break;

        case 10:
          displayUpdateRate++;
          if (displayUpdateRate > 6)
          {
            displayUpdateRate = 0;
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
   Setup pointers from the selected data settings for all 4 displayed values and LEDs
*/

void clearDisplay()
{
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}
void setupData()
{
  ptrData = selectData(dataSet[0]);
  ptrData2 = selectData(dataSet[1]);
  ptrData3 = selectData(dataSet[2]);
  ptrData4 = selectData(dataSet[3]);
  ptrDataLed1 = selectData(dataSet[4]);
  ptrDataLed2 = selectData(dataSet[5]);
  ptrDataLed4 = selectData(dataSet[6]);
  ptrData5 = selectData(dataSet[7]);
  ptrDataLed5 = selectData(dataSet[8]);
}

/*
   Save the selected data settings and enable them to be restored later
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
  preferences.putUInt("data5", dataSet[7]);
  preferences.putUInt("dataLED5", dataSet[8]);
  preferences.putUInt("ledType", ledType);
  preferences.putUInt("displayRate", displayUpdateRate);
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
   Verify that startup logos have been set. If not set them to the default value
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

float c2f(float c)
{
  float f = (c * 1.8) + 32;
  return f;
}

float kpa2psi(float k)
{
  float p = k * 0.1450377377;
  return p;
}

void saveSensorMinMax()
{

  // Multiply by 1000 so limits can keep .001 precision on restore
  int mult = 1000;
  testData.minimum = testData.minimum * mult;
  testData.maximum = testData.minimum * mult;
  testData.alertLow = testData.alertLow * mult;
  testData.alertHigh = testData.alertHigh * mult;
  afr.minimum = afr.minimum   * mult;
  afr.maximum =  afr.maximum     * mult;
  afr.alertLow = afr.alertLow     * mult;
  afr.alertHigh = afr.alertHigh     * mult;
  lambda1.minimum =  lambda1.minimum * mult;
  lambda1.maximum =   lambda1.maximum * mult;
  lambda1.alertLow =  lambda1.alertLow * mult;
  lambda1.alertHigh = lambda1.alertHigh * mult;
  lambda2.minimum =  lambda2.minimum * mult;
  lambda2.maximum =   lambda2.maximum * mult;
  lambda2.alertLow =  lambda2.alertLow * mult;
  lambda2.alertHigh = lambda2.alertHigh * mult;

  preferences.begin("limits", false);

  preferences.putInt("testMIN", testData.minimum);
  preferences.putInt("testMAX", testData.maximum);
  preferences.putInt("testLOW", testData.alertLow);
  preferences.putInt("testHI", testData.alertHigh);
  preferences.putInt("rpmMIN", rpm.minimum);
  preferences.putInt("rpmMAX", rpm.maximum);
  preferences.putInt("rpmLOW", rpm.alertLow);
  preferences.putInt("rpmHI", rpm.alertHigh);
  preferences.putInt("vssMIN", vss.minimum);
  preferences.putInt("vssMAX", vss.maximum);
  preferences.putInt("vssLOW", vss.alertLow);
  preferences.putInt("vssHI", vss.alertHigh);
  preferences.putInt("vssMPHMIN", vss_mph.minimum);
  preferences.putInt("vssMPHMAX", vss_mph.maximum);
  preferences.putInt("vssMPHLOW", vss_mph.alertLow);
  preferences.putInt("vssMPHHI", vss_mph.alertHigh);
  preferences.putInt("acceleratorMIN", accelerator.minimum);
  preferences.putInt("acceleratorMAX", accelerator.maximum);
  preferences.putInt("acceleratorLOW", accelerator.alertLow);
  preferences.putInt("acceleratorHI", accelerator.alertHigh);
  preferences.putInt("throttle1MIN", throttle1.minimum);
  preferences.putInt("throttle1MAX", throttle1.maximum);
  preferences.putInt("throttle1LOW", throttle1.alertLow);
  preferences.putInt("throttle1HI", throttle1.alertHigh);
  preferences.putInt("throttle1MIN", throttle2.minimum);
  preferences.putInt("throttle1MAX", throttle2.maximum);
  preferences.putInt("throttle1LOW", throttle2.alertLow);
  preferences.putInt("throttle1HI", throttle2.alertHigh);
  preferences.putInt("injDutyMIN", injectorDuty.minimum);
  preferences.putInt("injDutyMAX", injectorDuty.maximum);
  preferences.putInt("injDutyLOW", injectorDuty.alertLow);
  preferences.putInt("injDutyHI", injectorDuty.alertHigh);
  preferences.putInt("injPulseMIN", injectorPulse.minimum);
  preferences.putInt("injPulseMAX", injectorPulse.maximum);
  preferences.putInt("injPulseLOW", injectorPulse.alertLow);
  preferences.putInt("injPulseHI", injectorPulse.alertHigh);
  preferences.putInt("fuelPresHighMIN", fuelPressureHigh.minimum);
  preferences.putInt("fuelPresHighMAX", fuelPressureHigh.maximum);
  preferences.putInt("fuelPresHighLOW", fuelPressureHigh.alertLow);
  preferences.putInt("fuelPresHighHI", fuelPressureHigh.alertHigh);
  preferences.putInt("fuelPresLowMIN", fuelPressureLow.minimum);
  preferences.putInt("fuelPresLowMAX", fuelPressureLow.maximum);
  preferences.putInt("fuelPresLowLOW", fuelPressureLow.alertLow);
  preferences.putInt("fuelPresLowHI", fuelPressureLow.alertHigh);
  preferences.putInt("ethanolMIN", ethanol.minimum);
  preferences.putInt("ethanolMAX", ethanol.maximum);
  preferences.putInt("ethanolLOW", ethanol.alertLow);
  preferences.putInt("ethanolHI", ethanol.alertHigh);
  preferences.putInt("fuelTrimMIN", fuelTrim.minimum);
  preferences.putInt("fuelTrimMAX", fuelTrim.maximum);
  preferences.putInt("fuelTrimLOW", fuelTrim.alertLow);
  preferences.putInt("fuelTrimHI", fuelTrim.alertHigh);
  preferences.putInt("fuelMIN", fuel.minimum);
  preferences.putInt("fuelMAX", fuel.maximum);
  preferences.putInt("fuelLOW", fuel.alertLow);
  preferences.putInt("fuelHI", fuel.alertHigh);
  preferences.putInt("fuelUsedMIN", fuelConsumed.minimum);
  preferences.putInt("fuelUsedMAX", fuelConsumed.maximum);
  preferences.putInt("fuelUsedLOW", fuelConsumed.alertLow);
  preferences.putInt("fuelUsedHI", fuelConsumed.alertHigh);
  preferences.putInt("fuelFlowMIN", fuelConsumption.minimum);
  preferences.putInt("fuelFlowMAX", fuelConsumption.maximum);
  preferences.putInt("fuelFlowLOW", fuelConsumption.alertLow);
  preferences.putInt("fuelFlowHI", fuelConsumption.alertHigh);
  preferences.putInt("afrMIN", afr.minimum);
  preferences.putInt("afrMAX", afr.maximum);
  preferences.putInt("afrLOW", afr.alertLow);
  preferences.putInt("afrHI", afr.alertHigh);
  preferences.putInt("lambda1MIN", lambda1.minimum);
  preferences.putInt("lambda1MAX", lambda1.maximum);
  preferences.putInt("lambda1LOW", lambda1.alertLow);
  preferences.putInt("lambda1HI", lambda1.alertHigh);
  preferences.putInt("lambda2MIN", lambda1.minimum);
  preferences.putInt("lambda2MAX", lambda1.maximum);
  preferences.putInt("lambda2LOW", lambda1.alertLow);
  preferences.putInt("lambda2HI", lambda1.alertHigh);
  preferences.putInt("cltCMIN", coolantTemperature.minimum);
  preferences.putInt("cltCMAX", coolantTemperature.maximum);
  preferences.putInt("cltCLOW", coolantTemperature.alertLow);
  preferences.putInt("cltCHI", coolantTemperature.alertHigh);
  preferences.putInt("iatCMIN", intakeTemperature.minimum);
  preferences.putInt("iatCMAX", intakeTemperature.maximum);
  preferences.putInt("iatCLOW", intakeTemperature.alertLow);
  preferences.putInt("iatCHI", intakeTemperature.alertHigh);
  preferences.putInt("aux1CMIN", auxTemp1.minimum);
  preferences.putInt("aux1CMAX", auxTemp1.maximum);
  preferences.putInt("aux1CLOW", auxTemp1.alertLow);
  preferences.putInt("aux1CHI", auxTemp1.alertHigh);
  preferences.putInt("aux2CMIN", auxTemp2.minimum);
  preferences.putInt("aux2CMAX", auxTemp2.maximum);
  preferences.putInt("aux2CLOW", auxTemp2.alertLow);
  preferences.putInt("aux2CHI", auxTemp2.alertHigh);
  preferences.putInt("mcuCMIN", mcuTemp.minimum);
  preferences.putInt("mcuCMAX", mcuTemp.maximum);
  preferences.putInt("mcuCLOW", mcuTemp.alertLow);
  preferences.putInt("mcuCHI", mcuTemp.alertHigh);
  preferences.putInt("cltFMIN", coolantTemperature_f.minimum);
  preferences.putInt("cltFMAX", coolantTemperature_f.maximum);
  preferences.putInt("cltFLOW", coolantTemperature_f.alertLow);
  preferences.putInt("cltFHI", coolantTemperature_f.alertHigh);
  preferences.putInt("iatFMIN", intakeTemperature_f.minimum);
  preferences.putInt("iatFMAX", intakeTemperature_f.maximum);
  preferences.putInt("iatFLOW", intakeTemperature_f.alertLow);
  preferences.putInt("iatFHI", intakeTemperature_f.alertHigh);
  preferences.putInt("aux1FMIN", auxTemp1_f.minimum);
  preferences.putInt("aux1FMAX", auxTemp1_f.maximum);
  preferences.putInt("aux1FLOW", auxTemp1_f.alertLow);
  preferences.putInt("aux1FHI", auxTemp1_f.alertHigh);
  preferences.putInt("aux2FMIN", auxTemp2_f.minimum);
  preferences.putInt("aux2FMAX", auxTemp2_f.maximum);
  preferences.putInt("aux2FLOW", auxTemp2_f.alertLow);
  preferences.putInt("aux2FHI", auxTemp2_f.alertHigh);
  preferences.putInt("mapKPAMIN", manifoldPressure.minimum);
  preferences.putInt("mapKPAMAX", manifoldPressure.maximum);
  preferences.putInt("mapKPALOW", manifoldPressure.alertLow);
  preferences.putInt("mapKPAHI", manifoldPressure.alertHigh);
  preferences.putInt("oilKPAMIN", oilPressure.minimum);
  preferences.putInt("oilKPAMAX", oilPressure.maximum);
  preferences.putInt("oilKPALOW", oilPressure.alertLow);
  preferences.putInt("oilKPAHI", oilPressure.alertHigh);
  preferences.putInt("mapKPAMIN", manifoldPressure_psi.minimum);
  preferences.putInt("mapKPAMAX", manifoldPressure_psi.maximum);
  preferences.putInt("mapKPALOW", manifoldPressure_psi.alertLow);
  preferences.putInt("mapKPAHI", manifoldPressure_psi.alertHigh);
  preferences.putInt("oilKPAMIN", oilPressure_psi.minimum);
  preferences.putInt("oilKPAMAX", oilPressure_psi.maximum);
  preferences.putInt("oilKPALOW", oilPressure_psi.alertLow);
  preferences.putInt("oilKPAHI", oilPressure_psi.alertHigh);
  preferences.putInt("vvtMIN", vvtPosition.minimum);
  preferences.putInt("vvtMAX", vvtPosition.maximum);
  preferences.putInt("vvtLOW", vvtPosition.alertLow);
  preferences.putInt("vvtHI", vvtPosition.alertHigh);
  preferences.putInt("vvtEx1MIN", vvtExhaust1.minimum);
  preferences.putInt("vvtEx1MAX", vvtExhaust1.maximum);
  preferences.putInt("vvtEx1LOW", vvtExhaust1.alertLow);
  preferences.putInt("vvtEx1HI", vvtExhaust1.alertHigh);
  preferences.putInt("vvtEx2MIN", vvtExhaust2.minimum);
  preferences.putInt("vvtEx2MAX", vvtExhaust2.maximum);
  preferences.putInt("vvtEx2LOW", vvtExhaust2.alertLow);
  preferences.putInt("vvtEx2HI", vvtExhaust2.alertHigh);
  preferences.putInt("vvtIn1MIN", vvtIntake1.minimum);
  preferences.putInt("vvtIn1MAX", vvtIntake1.maximum);
  preferences.putInt("vvtIn1LOW", vvtIntake1.alertLow);
  preferences.putInt("vvtIn1HI", vvtIntake1.alertHigh);
  preferences.putInt("vvtIn2MIN", vvtIntake2.minimum);
  preferences.putInt("vvtIn2MAX", vvtIntake2.maximum);
  preferences.putInt("vvtIn2LOW", vvtIntake2.alertLow);
  preferences.putInt("vvtIn2HI", vvtIntake2.alertHigh);
  preferences.putInt("airMassMIN", airMass.minimum);
  preferences.putInt("airMassMAX", airMass.maximum);
  preferences.putInt("airMassLOW", airMass.alertLow);
  preferences.putInt("airMassHI", airMass.alertHigh);
  preferences.putInt("estAirMIN", estimatedAirflow.minimum);
  preferences.putInt("estAirMAX", estimatedAirflow.maximum);
  preferences.putInt("estAirLOW", estimatedAirflow.alertLow);
  preferences.putInt("estAirHI", estimatedAirflow.alertHigh);
  preferences.putInt("batVMIN", batteryVoltage.minimum);
  preferences.putInt("batVMAX", batteryVoltage.maximum);
  preferences.putInt("batVLOW", batteryVoltage.alertLow);
  preferences.putInt("batVHI", batteryVoltage.alertHigh);

  preferences.end();

  testData.minimum = testData.minimum / mult;
  testData.maximum = testData.minimum / mult;
  testData.alertLow = testData.alertLow / mult;
  testData.alertHigh = testData.alertHigh / mult;
  afr.minimum = afr.minimum   / mult;
  afr.maximum =  afr.maximum     / mult;
  afr.alertLow = afr.alertLow     / mult;
  afr.alertHigh = afr.alertHigh     / mult;
  lambda1.minimum =  lambda1.minimum / mult;
  lambda1.maximum =   lambda1.maximum / mult;
  lambda1.alertLow =  lambda1.alertLow / mult;
  lambda1.alertHigh = lambda1.alertHigh / mult;
  lambda2.minimum =  lambda2.minimum / mult;
  lambda2.maximum =   lambda2.maximum / mult;
  lambda2.alertLow =  lambda2.alertLow / mult;
  lambda2.alertHigh = lambda2.alertHigh / mult;

}

void restoreSensorMinMax()
{
  preferences.begin("limits", true);


  testData.minimum =    preferences.getInt("testMIN", testData.minimum);
  testData.maximum =    preferences.getInt("testMAX", testData.maximum);
  testData.alertLow =   preferences.getInt("testLOW", testData.alertLow);
  testData.alertHigh =  preferences.getInt("testHI", testData.alertHigh);
  rpm.minimum = preferences.getInt("rpmMIN", rpm.minimum);
  rpm.maximum = preferences.getInt("rpmMAX", rpm.maximum);
  rpm.alertLow = preferences.getInt("rpmLOW", rpm.alertLow);
  rpm.alertHigh = preferences.getInt("rpmHI", rpm.alertHigh);
  vss.minimum = preferences.getInt("vssMIN", vss.minimum);
  vss.maximum = preferences.getInt("vssMAX", vss.maximum);
  vss.alertLow = preferences.getInt("vssLOW", vss.alertLow);
  vss.alertHigh = preferences.getInt("vssHI", vss.alertHigh);
  vss_mph.minimum = preferences.getInt("vssMPHMIN", vss_mph.minimum);
  vss_mph.maximum = preferences.getInt("vssMPHMAX", vss_mph.maximum);
  vss_mph.alertLow = preferences.getInt("vssMPHLOW", vss_mph.alertLow);
  vss_mph.alertHigh = preferences.getInt("vssMPHHI", vss_mph.alertHigh);
  accelerator.minimum = preferences.getInt("acceleratorMIN", accelerator.minimum);
  accelerator.maximum = preferences.getInt("acceleratorMAX", accelerator.maximum);
  accelerator.alertLow = preferences.getInt("acceleratorLOW", accelerator.alertLow);
  accelerator.alertHigh = preferences.getInt("acceleratorHI", accelerator.alertHigh);
  throttle1.minimum = preferences.getInt("throttle1MIN", throttle1.minimum);
  throttle1.maximum = preferences.getInt("throttle1MAX", throttle1.maximum);
  throttle1.alertLow = preferences.getInt("throttle1LOW", throttle1.alertLow);
  throttle1.alertHigh = preferences.getInt("throttle1HI", throttle1.alertHigh);
  throttle2.minimum = preferences.getInt("throttle1MIN", throttle2.minimum);
  throttle2.maximum = preferences.getInt("throttle1MAX", throttle2.maximum);
  throttle2.alertLow = preferences.getInt("throttle1LOW", throttle2.alertLow);
  throttle2.alertHigh = preferences.getInt("throttle1HI", throttle2.alertHigh);
  injectorDuty.minimum = preferences.getInt("injDutyMIN", injectorDuty.minimum);
  injectorDuty.maximum = preferences.getInt("injDutyMAX", injectorDuty.maximum);
  injectorDuty.alertLow = preferences.getInt("injDutyLOW", injectorDuty.alertLow);
  injectorDuty.alertHigh = preferences.getInt("injDutyHI", injectorDuty.alertHigh);
  injectorPulse.minimum = preferences.getInt("injPulseMIN", injectorPulse.minimum);
  injectorPulse.maximum = preferences.getInt("injPulseMAX", injectorPulse.maximum);
  injectorPulse.alertLow = preferences.getInt("injPulseLOW", injectorPulse.alertLow);
  injectorPulse.alertHigh = preferences.getInt("injPulseHI", injectorPulse.alertHigh);
  fuelPressureHigh.minimum = preferences.getInt("fuelPresHighMIN", fuelPressureHigh.minimum);
  fuelPressureHigh.maximum = preferences.getInt("fuelPresHighMAX", fuelPressureHigh.maximum);
  fuelPressureHigh.alertLow = preferences.getInt("fuelPresHighLOW", fuelPressureHigh.alertLow);
  fuelPressureHigh.alertHigh = preferences.getInt("fuelPresHighHI", fuelPressureHigh.alertHigh);
  fuelPressureLow.minimum = preferences.getInt("fuelPresLowMIN", fuelPressureLow.minimum);
  fuelPressureLow.maximum = preferences.getInt("fuelPresLowMAX", fuelPressureLow.maximum);
  fuelPressureLow.alertLow = preferences.getInt("fuelPresLowLOW", fuelPressureLow.alertLow);
  fuelPressureLow.alertHigh = preferences.getInt("fuelPresLowHI", fuelPressureLow.alertHigh);
  ethanol.minimum = preferences.getInt("ethanolMIN", ethanol.minimum);
  ethanol.maximum = preferences.getInt("ethanolMAX", ethanol.maximum);
  ethanol.alertLow = preferences.getInt("ethanolLOW", ethanol.alertLow);
  ethanol.alertHigh = preferences.getInt("ethanolHI", ethanol.alertHigh);
  fuelTrim.minimum = preferences.getInt("fuelTrimMIN", fuelTrim.minimum);
  fuelTrim.maximum = preferences.getInt("fuelTrimMAX", fuelTrim.maximum);
  fuelTrim.alertLow = preferences.getInt("fuelTrimLOW", fuelTrim.alertLow);
  fuelTrim.alertHigh = preferences.getInt("fuelTrimHI", fuelTrim.alertHigh);
  fuel.minimum = preferences.getInt("fuelMIN", fuel.minimum);
  fuel.maximum = preferences.getInt("fuelMAX", fuel.maximum);
  fuel.alertLow = preferences.getInt("fuelLOW", fuel.alertLow);
  fuel.alertHigh = preferences.getInt("fuelHI", fuel.alertHigh);
  fuelConsumed.minimum = preferences.getInt("fuelUsedMIN", fuelConsumed.minimum);
  fuelConsumed.maximum = preferences.getInt("fuelUsedMAX", fuelConsumed.maximum);
  fuelConsumed.alertLow = preferences.getInt("fuelUsedLOW", fuelConsumed.alertLow);
  fuelConsumed.alertHigh = preferences.getInt("fuelUsedHI", fuelConsumed.alertHigh);
  fuelConsumption.minimum = preferences.getInt("fuelFlowMIN", fuelConsumption.minimum);
  fuelConsumption.maximum = preferences.getInt("fuelFlowMAX", fuelConsumption.maximum);
  fuelConsumption.alertLow = preferences.getInt("fuelFlowLOW", fuelConsumption.alertLow);
  fuelConsumption.alertHigh = preferences.getInt("fuelFlowHI", fuelConsumption.alertHigh);
  afr.minimum =       preferences.getInt("afrMIN", afr.minimum);
  afr.maximum =       preferences.getInt("afrMAX", afr.maximum);
  afr.alertLow =      preferences.getInt("afrLOW", afr.alertLow);
  afr.alertHigh =     preferences.getInt("afrHI", afr.alertHigh);
  lambda1.minimum =   preferences.getInt("lambda1MIN", lambda1.minimum);
  lambda1.maximum =   preferences.getInt("lambda1MAX", lambda1.maximum);
  lambda1.alertLow =  preferences.getInt("lambda1LOW", lambda1.alertLow);
  lambda1.alertHigh = preferences.getInt("lambda1HI", lambda1.alertHigh);
  lambda2.minimum =   preferences.getInt("lambda2MIN", lambda1.minimum);
  lambda2.maximum =   preferences.getInt("lambda2MAX", lambda1.maximum);
  lambda2.alertLow =  preferences.getInt("lambda2LOW", lambda1.alertLow);
  lambda2.alertHigh = preferences.getInt("lambda2HI", lambda1.alertHigh);
  coolantTemperature.minimum = preferences.getInt("cltCMIN", coolantTemperature.minimum);
  coolantTemperature.maximum = preferences.getInt("cltCMAX", coolantTemperature.maximum);
  coolantTemperature.alertLow = preferences.getInt("cltCLOW", coolantTemperature.alertLow);
  coolantTemperature.alertHigh = preferences.getInt("cltCHI", coolantTemperature.alertHigh);
  intakeTemperature.minimum = preferences.getInt("iatCMIN", intakeTemperature.minimum);
  intakeTemperature.maximum = preferences.getInt("iatCMAX", intakeTemperature.maximum);
  intakeTemperature.alertLow = preferences.getInt("iatCLOW", intakeTemperature.alertLow);
  intakeTemperature.alertHigh = preferences.getInt("iatCHI", intakeTemperature.alertHigh);
  auxTemp1.minimum = preferences.getInt("aux1CMIN", auxTemp1.minimum);
  auxTemp1.maximum = preferences.getInt("aux1CMAX", auxTemp1.maximum);
  auxTemp1.alertLow = preferences.getInt("aux1CLOW", auxTemp1.alertLow);
  auxTemp1.alertHigh = preferences.getInt("aux1CHI", auxTemp1.alertHigh);
  auxTemp2.minimum = preferences.getInt("aux2CMIN", auxTemp2.minimum);
  auxTemp2.maximum = preferences.getInt("aux2CMAX", auxTemp2.maximum);
  auxTemp2.alertLow = preferences.getInt("aux2CLOW", auxTemp2.alertLow);
  auxTemp2.alertHigh = preferences.getInt("aux2CHI", auxTemp2.alertHigh);
  mcuTemp.minimum = preferences.getInt("mcuCMIN", mcuTemp.minimum);
  mcuTemp.maximum = preferences.getInt("mcuCMAX", mcuTemp.maximum);
  mcuTemp.alertLow = preferences.getInt("mcuCLOW", mcuTemp.alertLow);
  mcuTemp.alertHigh = preferences.getInt("mcuCHI", mcuTemp.alertHigh);
  coolantTemperature_f.minimum = preferences.getInt("cltFMIN", coolantTemperature_f.minimum);
  coolantTemperature_f.maximum = preferences.getInt("cltFMAX", coolantTemperature_f.maximum);
  coolantTemperature_f.alertLow = preferences.getInt("cltFLOW", coolantTemperature_f.alertLow);
  coolantTemperature_f.alertHigh = preferences.getInt("cltFHI", coolantTemperature_f.alertHigh);
  coolantTemperature_f.minimum = preferences.getInt("iatFMIN", intakeTemperature_f.minimum);
  coolantTemperature_f.maximum = preferences.getInt("iatFMAX", intakeTemperature_f.maximum);
  coolantTemperature_f.alertLow = preferences.getInt("iatFLOW", intakeTemperature_f.alertLow);
  coolantTemperature_f.alertHigh = preferences.getInt("iatFHI", intakeTemperature_f.alertHigh);
  auxTemp1_f.minimum = preferences.getInt("aux1FMIN", auxTemp1_f.minimum);
  auxTemp1_f.maximum = preferences.getInt("aux1FMAX", auxTemp1_f.maximum);
  auxTemp1_f.alertLow = preferences.getInt("aux1FLOW", auxTemp1_f.alertLow);
  auxTemp1_f.alertHigh = preferences.getInt("aux1FHI", auxTemp1_f.alertHigh);
  auxTemp2_f.minimum = preferences.getInt("aux2FMIN", auxTemp2_f.minimum);
  auxTemp2_f.maximum = preferences.getInt("aux2FMAX", auxTemp2_f.maximum);
  auxTemp2_f.alertLow = preferences.getInt("aux2FLOW", auxTemp2_f.alertLow);
  auxTemp2_f.alertHigh = preferences.getInt("aux2FHI", auxTemp2_f.alertHigh);
  manifoldPressure.minimum = preferences.getInt("mapKPAMIN", manifoldPressure.minimum);
  manifoldPressure.maximum = preferences.getInt("mapKPAMAX", manifoldPressure.maximum);
  manifoldPressure.alertLow = preferences.getInt("mapKPALOW", manifoldPressure.alertLow);
  manifoldPressure.alertHigh = preferences.getInt("mapKPAHI", manifoldPressure.alertHigh);
  oilPressure.minimum = preferences.getInt("oilKPAMIN", oilPressure.minimum);
  oilPressure.maximum = preferences.getInt("oilKPAMAX", oilPressure.maximum);
  oilPressure.alertLow = preferences.getInt("oilKPALOW", oilPressure.alertLow);
  oilPressure.alertHigh = preferences.getInt("oilKPAHI", oilPressure.alertHigh);
  manifoldPressure_psi.minimum = preferences.getInt("mapKPAMIN", manifoldPressure_psi.minimum);
  manifoldPressure_psi.maximum = preferences.getInt("mapKPAMAX", manifoldPressure_psi.maximum);
  manifoldPressure_psi.alertLow = preferences.getInt("mapKPALOW", manifoldPressure_psi.alertLow);
  manifoldPressure_psi.alertHigh = preferences.getInt("mapKPAHI", manifoldPressure_psi.alertHigh);
  oilPressure_psi.minimum = preferences.getInt("oilKPAMIN", oilPressure_psi.minimum);
  oilPressure_psi.maximum = preferences.getInt("oilKPAMAX", oilPressure_psi.maximum);
  oilPressure_psi.alertLow = preferences.getInt("oilKPALOW", oilPressure_psi.alertLow);
  oilPressure_psi.alertHigh = preferences.getInt("oilKPAHI", oilPressure_psi.alertHigh);
  vvtPosition.minimum = preferences.getInt("vvtMIN", vvtPosition.minimum);
  vvtPosition.maximum = preferences.getInt("vvtMAX", vvtPosition.maximum);
  vvtPosition.alertLow = preferences.getInt("vvtLOW", vvtPosition.alertLow);
  vvtPosition.alertHigh = preferences.getInt("vvtHI", vvtPosition.alertHigh);
  vvtExhaust1.minimum = preferences.getInt("vvtEx1MIN", vvtExhaust1.minimum);
  vvtExhaust1.maximum = preferences.getInt("vvtEx1MAX", vvtExhaust1.maximum);
  vvtExhaust1.alertLow = preferences.getInt("vvtEx1LOW", vvtExhaust1.alertLow);
  vvtExhaust1.alertHigh = preferences.getInt("vvtEx1HI", vvtExhaust1.alertHigh);
  vvtExhaust2.minimum = preferences.getInt("vvtEx2MIN", vvtExhaust2.minimum);
  vvtExhaust2.maximum = preferences.getInt("vvtEx2MAX", vvtExhaust2.maximum);
  vvtExhaust2.alertLow = preferences.getInt("vvtEx2LOW", vvtExhaust2.alertLow);
  vvtExhaust2.alertHigh = preferences.getInt("vvtEx2HI", vvtExhaust2.alertHigh);
  vvtIntake1.minimum = preferences.getInt("vvtIn1MIN", vvtIntake1.minimum);
  vvtIntake1.maximum = preferences.getInt("vvtIn1MAX", vvtIntake1.maximum);
  vvtIntake1.alertLow = preferences.getInt("vvtIn1LOW", vvtIntake1.alertLow);
  vvtIntake1.alertHigh = preferences.getInt("vvtIn1HI", vvtIntake1.alertHigh);
  vvtIntake2.minimum = preferences.getInt("vvtIn2MIN", vvtIntake2.minimum);
  vvtIntake2.maximum = preferences.getInt("vvtIn2MAX", vvtIntake2.maximum);
  vvtIntake2.alertLow = preferences.getInt("vvtIn2LOW", vvtIntake2.alertLow);
  vvtIntake2.alertHigh = preferences.getInt("vvtIn2HI", vvtIntake2.alertHigh);
  airMass.minimum = preferences.getInt("airMassMIN", airMass.minimum);
  airMass.maximum = preferences.getInt("airMassMAX", airMass.maximum);
  airMass.alertLow = preferences.getInt("airMassLOW", airMass.alertLow);
  airMass.alertHigh = preferences.getInt("airMassHI", airMass.alertHigh);
  estimatedAirflow.minimum = preferences.getInt("estAirMIN", estimatedAirflow.minimum);
  estimatedAirflow.maximum = preferences.getInt("estAirMAX", estimatedAirflow.maximum);
  estimatedAirflow.alertLow = preferences.getInt("estAirLOW", estimatedAirflow.alertLow);
  estimatedAirflow.alertHigh = preferences.getInt("estAirHI", estimatedAirflow.alertHigh);
  batteryVoltage.minimum =    preferences.getInt("batVMIN", batteryVoltage.minimum);
  batteryVoltage.maximum =    preferences.getInt("batVMAX", batteryVoltage.maximum);
  batteryVoltage.alertLow =   preferences.getInt("batVLOW", batteryVoltage.alertLow);
  batteryVoltage.alertHigh =  preferences.getInt("batVHI", batteryVoltage.alertHigh);

  preferences.end();

  // Restore properly from int
  int mult = 1000;
  testData.minimum = testData.minimum / mult;
  testData.maximum = testData.minimum / mult;
  testData.alertLow = testData.alertLow / mult;
  testData.alertHigh = testData.alertHigh / mult;
  afr.minimum = afr.minimum   / mult;
  afr.maximum =  afr.maximum     / mult;
  afr.alertLow = afr.alertLow     / mult;
  afr.alertHigh = afr.alertHigh     / mult;
  lambda1.minimum =  lambda1.minimum / mult;
  lambda1.maximum =   lambda1.maximum / mult;
  lambda1.alertLow =  lambda1.alertLow / mult;
  lambda1.alertHigh = lambda1.alertHigh / mult;
  lambda2.minimum =  lambda2.minimum / mult;
  lambda2.maximum =   lambda2.maximum / mult;
  lambda2.alertLow =  lambda2.alertLow / mult;
  lambda2.alertHigh = lambda2.alertHigh / mult;

}

void setGaugeConfig(int setType, int setGauge, int setSensor)
{
  switch (setType)
  {
    // Data shown

    case 0:
      switch (setGauge)
      {
        // 1x gauge
        case 0:
          dataSet[0] = setSensor;
          break;
        // 2x Gauge
        case 1:
          dataSet[1] = setSensor;
          break;
        // 4x data 3
        case 2:
          dataSet[2] = setSensor;
          break;
        // 4x data 4
        case 3:
          dataSet[3] = setSensor;
          break;
        // Alt 1x data
        case 4:
          dataSet[7] = setSensor;
          break;
      }
      break;

    case 1:
      switch (setGauge)
      {
        // 1x gauge
        case 0:
          dataSet[4] = setSensor;
          break;
        // 2x Gauge
        case 1:
          dataSet[5] = setSensor;
          break;
        // 4x data 3
        case 2:
          dataSet[6] = setSensor;
          break;
        // 4x data 4
        case 3:
          dataSet[6] = setSensor;
          break;
        // Alt 1x data
        case 4:
          dataSet[9] = setSensor;
          break;
      }
      break;

    default:
      break;
  }

  setupData();
}

void setLedMode(int setLed) {
  ledType = setLed;
  setupData();
}

void setDisplayRate(int dispRate) {
  displayUpdateRate = dispRate;
  setupData();
}

void setSensorMinMax(int sensor, int limit, float newValue)
{

  newSensorConfig = selectData(sensor);

  switch (limit)
  {
    case 1:
      newSensorConfig->minimum = newValue;
      break;

    case 2:
      newSensorConfig->maximum = newValue;
      break;

    case 3:
      newSensorConfig->alertLow = newValue;
      break;

    case 4:
      newSensorConfig->alertHigh = newValue;
      break;

    default:
      break;
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
   Indicate that button #1 has been pressed
   It is debounced to reduce false presses
*/
void buttonISR()
{
  if ((millis() - lastInput) > debounceTimer)
  {
    buttonPress = 1;
  }
}

/*
   Indicate that button #2 has been pressed
   It is debounced to reduce false presses
*/
void buttonISR2()
{
  if ((millis() - lastInput2) > debounceTimer)
  {
    buttonPress2 = 1;
  }
}

/*
   Cycle to the next gauge to display when button 1 is pressed
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
   Select the next style of gauge and is dependent on the current gauge
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
   Return the percentage of the current value of its total range defined by the input min/max values
*/
int getPercent(float current, float minimum, float maximum)
{

  float percent = (100 * (current - minimum) / (maximum - minimum));
  int p = (int) percent;

  if (p > 100){
    p = 100;
  }

  if (p < 0){
    p = 0;
  }
  
  return (p);
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
      ledOff();
      sequentialLedAll(percent);
      break;

    case 1:
      ledOff();
      sequentialLed(percent);
      break;

    case 2:
      ledOff();
      singleLedAll(percent);
      break;

    case 3:
      ledOff();
      singleLed(percent);
      break;

    case 4:
      ledOff();
      break;

    case 5:
      indLedOff();
      sequentialLed(percent);

      if (newWarning == true)
      {
        digitalWrite(LED_11, HIGH);
      }
      else
      {
        digitalWrite(LED_11, LOW);
      }

      if (data->scaledValue > data->alertHigh || data->scaledValue < data->alertLow)
      {
        digitalWrite(LED_12, HIGH);
      }
      else
      {
        digitalWrite(LED_12, LOW);
      }
      break;

    case 6:
      indLedOff();
      singleLed(percent);

      if (newWarning == true)
      {
        digitalWrite(LED_11, HIGH);
      }
      else
      {
        digitalWrite(LED_11, LOW);
      }
      if (data->scaledValue > data->alertHigh || data->scaledValue < data->alertLow)
      {
        digitalWrite(LED_12, HIGH);
      }
      else
      {
        digitalWrite(LED_12, LOW);
      }
      break;

    default:
      ledOff();
      sequentialLedAll(percent);
      break;
  }
}

/*
   Print the selected gauge and data on the display
*/

void printLeds(int g)
{

  switch (g)
  {
    case 0:
      renderLeds(ledType, ptrDataLed1);
      break;

    case 1:
      renderLeds(ledType, ptrDataLed2);
      break;

    case 2:
      renderLeds(ledType, ptrDataLed4);
      break;

    case 3:
      renderLeds(ledType, ptrDataLed5);
      break;

    default:
      ledOff();
      break;
  }
}

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

      u8g2.sendBuffer();

      break;

    case 3:
      // 1x Sensor Gauge Alternate
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.setCursor(0, 0);
      printDataNameAndUnits(ptrData5);

      u8g2.setFont(u8g2_font_fub35_tf);
      u8g2.setCursor(0, 26);

      printDataFormatted(ptrData5);

      if (ptrData5->scaleMultiplier == 0)
      {
        u8g2.clearBuffer();
      }

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
      ;
      u8g2.sendBuffer();
      break;

    // WIFI
    case 10:
      // OTA / WiFi Setup
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.clearBuffer();
      u8g2.setCursor(0, 0);
      u8g2.print("WiFi");

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
        u8g2.print("<-     disable    ->");
        ota();
      }
      else
      {
        u8g2.print(" Disabled");
        u8g2.setCursor(0, 16);
        u8g2.print("Use to update and to");
        u8g2.setCursor(0, 28);
        u8g2.print("configure settings");
        u8g2.setCursor(0, 53);
        u8g2.print("Press -> to enable");
      }
#else
      u8g2.setCursor(0, 15);
      u8g2.print("Feature disabled");
      u8g2.setCursor(0, 30);
      u8g2.print("Update required!");
#endif

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
      gaugeType = 0;
      gauge = 0;
      reset = 0;
  }
}

/*
   Returns a pointer to the selected data type to be used
*/

sensorData *selectData(int g)
{

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
      return &auxTemp1_f;
      break;

    case 23:
      return &auxTemp2;
      break;

     case 24:
      return &auxTemp2_f;
      break;

    case 25:
      return &batteryVoltage;
      break;

    case 26:
      return &ethanol;
      break;

    case 27:
      return &lambda1;
      break;

    case 28:
      return &lambda2;
      break;

    case 29:
      return &fuelPressureLow;
      break;

    case 30:
      return &fuelPressureHigh;
      break;

    case 31:
      return &fuelConsumed;
      break;

    case 32:
      return &fuelConsumption;
      break;

    case 33:
      return &fuelTrim;
      break;

    case 34:
      return &vvtIntake1;
      break;

    case 35:
      return &vvtIntake2;
      break;

    case 36:
      return &vvtExhaust1;
      break;

    case 37:
      return &vvtExhaust2;
      break;

    case 38:
      return &oilPressure;
      break;

    case 39:
      return &oilPressure_psi;
      break;

    case 40:
      return &noData;
      break;

    case 41:
      return &testData;
      break;

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
   This function is used to setup filtering on incoming CAN messages, restricting to only standard frames (11bit)
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
   This function is used to initialize CAN, restricting to only standard frames (11bit)
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
   This function is used to create a task to assign to core 0, to utilize the second core for processing CAN messages
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

  checkNVS();

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
  dataSet[7] = preferences.getUInt("data5", 0);
  dataSet[8] = preferences.getUInt("dataLED5", 0);
  ledType = preferences.getUInt("ledType", 0);
  displayUpdateRate = preferences.getUInt("displayRate", 0);

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

  restoreSensorMinMax();
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

#ifdef WIFI_ALWAYS_ACTIVE
  otaSetup();
#else
#endif
}

void loop()
{

  // Slow down display updates to improve legibility
  if (gaugeType < 5)
  {
    if ((millis() - lastDisplayUpdate) > displayUpdateTime[displayUpdateRate])
    {
      // Update the display with the currently selected data
      lastDisplayUpdate = millis();
      printData(gaugeType);
    }
    else
    {
      u8g2.sendBuffer();
    }
  }
  else
  {
    // If data is not being displayed do not slow down
    printData(gaugeType);
  }

  printLeds(gaugeType);

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

#ifdef WIFI_ALWAYS_ACTIVE
  ota();
#else
#endif


  if (readyToUpdateLimits == true)
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 0);
    u8g2.print("Updating Limit Configuration");
    u8g2.sendBuffer();
    setSensorMinMax(selectedSensor, selectedLimit, inputNewValue);
    saveSensorMinMax();
    readyToUpdateLimits = false;
  }

  if (readyToUpdateGaugeConfig == true)
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 0);
    u8g2.print("Updating Gauge Configuration");
    u8g2.sendBuffer();
    setGaugeConfig(selectedType, selectedGauge, selectedSensor);
    saveDataSettings();
    readyToUpdateGaugeConfig = false;
  }

  if (readyToUpdateLedMode == true)
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 0);
    u8g2.print("Updating LED Mode");
    u8g2.sendBuffer();
    setLedMode(selectedLedMode);
    saveDataSettings();
    readyToUpdateLedMode = false;
  }

  if (readyToUpdateDisplayRate == true)
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 0);
    u8g2.print("Updating Display Rate");
    u8g2.sendBuffer();
    setDisplayRate(selectedDisplayRate);
    saveDataSettings();
    readyToUpdateDisplayRate = false;
  }

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
