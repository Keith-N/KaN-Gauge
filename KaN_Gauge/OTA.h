#include <Preferences.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <WiFiAP.h>

#include "Pages.h"

// WiFi OTA
const char *host = "KaN Gauge";
const char *ssid = "KaN Gauge";
const char *password = "update1234";

WebServer server(80);

int selectedSensor;
int selectedLimit;
int selectedType;
int selectedGauge;
int selectedLedMode;
int selectedDisplayRate;

float inputNewValue;

bool readyToUpdateLimits = false;
bool readyToUpdateGaugeConfig = false;
bool readyToUpdateDisplayRate = false;
bool readyToUpdateLedMode = false;



bool opened = false;



/*
    Service any client connections to the gauge
*/

void ota(void)
{
  server.handleClient();
}


/*
    When JS is required send the GZ JQuery 3.2.1
*/
void onJavaScript(void)
{
  Serial.println("onJavaScript(void)");
  server.setContentLength(jquery_min_js_v3_2_1_gz_len);
  server.sendHeader(F("Content-Encoding"), F("gzip"));
  server.send_P(200, "text/javascript", jquery_min_js_v3_2_1_gz, jquery_min_js_v3_2_1_gz_len);
}

/*
    Setup the WiFi connection and prepare
*/

void otaSetup(void)
{


  WiFi.softAP(ssid, password);

  server.on("/jquery.min.js", HTTP_GET, onJavaScript);

  server.on("/reset", HTTP_GET, [] {
    ESP.restart();
  });

  server.on("/", HTTP_GET, [] {
    server.send(200, "text/html", startPage);
  });

  server.on("/updateStart", HTTP_GET, [] {
    server.send(200, "text/html", update1);
  });


  server.on("/gaugeConfig", HTTP_GET, [] {
    server.send(200, "text/html", gaugeConfigPage);
  });


  server.on("/setLimitConfig", HTTP_GET, [] {
    String a, b, c;
    a = server.arg(0);
    b = server.arg(1);
    c = server.arg(2);

    selectedSensor = (int) (a.toFloat());
    selectedLimit = (int) (b.toFloat());
    inputNewValue = c.toFloat();

    readyToUpdateLimits = true;
    server.send(200, "text/html", gaugeConfigPage);
  });


  server.on("/setGaugeConfig", HTTP_GET, [] {
    String a, b, c;
    a = server.arg(0);
    b = server.arg(1);
    c = server.arg(2);

    selectedType = (int) (a.toFloat());
    selectedGauge = (int) (b.toFloat());
    selectedSensor = (int) (c.toFloat());

    readyToUpdateGaugeConfig = true;
    server.send(200, "text/html", gaugeConfigPage);
  });

  server.on("/setLedMode", HTTP_GET, [] {
    String a;
    a = server.arg(0);

    selectedLedMode = (int) (a.toFloat());

    readyToUpdateLedMode = true;
    server.send(200, "text/html", gaugeConfigPage);
  });

  server.on("/setDisplayRate", HTTP_GET, [] {
    String a;
    a = server.arg(0);

    selectedDisplayRate = (int) (a.toFloat());

    readyToUpdateDisplayRate = true;
    server.send(200);
  });

  server.on("/update", HTTP_POST, []()
  {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  },

  []() {

    HTTPUpload &upload = server.upload();

    // Uploading File
    if (upload.status == UPLOAD_FILE_START)
    {
      digitalWrite(LED_11, HIGH);

      // Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN))
      { // start with max available size
        Update.printError(Serial);
      }
    }
    // File Uploaded, flash
    else if (upload.status == UPLOAD_FILE_WRITE)
    {

      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
      {
        Update.printError(Serial);
      }
    }

    // Update finished
    else if (upload.status == UPLOAD_FILE_END)
    {
      digitalWrite(LED_12, HIGH);
      // If update is a success
      if (Update.end(true))
      {
      }
      // If update fails
      else
      {
        // Update.printError(Serial);
      }
    }
  });

  //server.onNotFound(notFound);
  server.begin();

}
