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

const char* PARAM_MESSAGE = "message";
const char* INPUT_1 = "A";
const char* INPUT_2 = "B";
const char* INPUT_3 = "C";

int selectedSensor;
int selectedLimit;
int selectedType;
int selectedGauge;
int selectedLedType;
int selectedDisplayRate;

String inputNewValue;
float inputNewValueF;

bool readyToSetLimits = false;
bool updatedLimits = false;
bool readyToUpdateGauge = false;

bool opened = false;

void ota(void)
{
    server.handleClient();
}

void onJavaScript(void)
{
    Serial.println("onJavaScript(void)");
    server.setContentLength(jquery_min_js_v3_2_1_gz_len);
    server.sendHeader(F("Content-Encoding"), F("gzip"));
    server.send_P(200, "text/javascript", jquery_min_js_v3_2_1_gz, jquery_min_js_v3_2_1_gz_len);
}

void otaSetup(void)
{
  
    
    WiFi.softAP(ssid, password);

    server.on("/jquery.min.js", HTTP_GET, onJavaScript);
    
    server.on("/reset", HTTP_GET, []{
      ESP.restart();
    });
    
    server.on("/", HTTP_GET, []{
        server.send(200, "text/html", startPage);
    });
    
    server.on("/updateStart", HTTP_GET, []{
        server.send(200, "text/html", update1);
    });

    server.on("/ledConfig", HTTP_GET, []{
        server.send(200, "text/html", ledConfigPage);
    });

    server.on("/gaugeConfig", HTTP_GET, []{
        server.send(200, "text/html", gaugeConfigPage);
    });    
    
    server.on("/getConfig", HTTP_GET, []{

    String a, b, c, d;
      a = server.arg(0);
      b = server.arg(1);
      c = server.arg(2);
      
     selectedSensor = (int) (a.toFloat());
     selectedLimit = (int) (b.toFloat());
     inputNewValueF = c.toFloat();
     readyToSetLimits = true;
     updatedLimits = true;
     
     server.send(200, "text/html", ledConfigPage);
        
    });

    server.on("/setGaugeConfig", HTTP_GET, []{

    String a, b, c, d, e;
      a = server.arg(0);
      b = server.arg(1);
      c = server.arg(2);
      d = server.arg(3);
      e = server.arg(4);
      
      
     selectedType = (int) (a.toFloat());
     selectedGauge = (int) (b.toFloat());
     selectedSensor = (int) (c.toFloat());
     selectedLedType = (int) (d.toFloat());
     selectedDisplayRate = (int) (e.toFloat());

     readyToUpdateGauge = true;
     server.send(200, "text/html", gaugeConfigPage);
        
    });

    server.on(
        "/update", HTTP_POST, []()
        {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); },

        []()

        {
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
