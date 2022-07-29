
#include <U8g2lib.h>
#include "BMP.h"

//Setup U8G2 for 128x64 SH1105 OLED, Name u8g2
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, I2C_SCL, I2C_SDA);
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, I2C_SCL, I2C_SDA);
//U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, I2C_SCL, I2C_SDA, U8X8_PIN_NONE);

// BMP data for displaying on OLED
#define dispWidth 128
#define dispHeight 64

// -----------------------------------      OLED Functions     ---------------------------------------------


void oledSetup(void) {
  // Setup default configuration for font
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}


void printBMP_BMM(){
  u8g2.clearBuffer();
  u8g2.drawXBM(0,0,dispWidth,dispHeight,BMP_BMM);
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0,0);
  //u8g2.print(BUILD);
  u8g2.sendBuffer();
}

void printBMP_GG(){
  u8g2.clearBuffer();
  u8g2.drawXBM(0,0,dispWidth,dispHeight,BMP_GG);
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0,0);
  //u8g2.print(BUILD);
  u8g2.sendBuffer();
}

void printGitQR(){
  u8g2.clearBuffer();
  u8g2.drawXBM(0,0,dispWidth,dispHeight,GITHUB_QR);
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0,0);
  u8g2.print("KaN");
  u8g2.setCursor(10,10);
  u8g2.print("Gauge");
  u8g2.setCursor(0,30);
  u8g2.print(BUILD);

  #ifdef DEBUG_BUILD
    u8g2.setCursor(0,50);
    u8g2.print("DEBUG");
  #endif
  u8g2.sendBuffer();
}

void printBuild(){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0,0);
  u8g2.print(BUILD);
  u8g2.sendBuffer();
}


void printInitialStart(){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0,0);
  u8g2.print("Initializing ...");
  
  #ifdef RESET_STORED
    u8g2.setCursor(0,25);
    u8g2.print("Resetting data!");
  #endif
  
  u8g2.sendBuffer();
}

void printBMP_KaN(){
  u8g2.clearBuffer();
  u8g2.drawXBM(0,0,dispWidth,dispHeight,BMP_KaN);
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0,0);
  //u8g2.print(BUILD);
  u8g2.sendBuffer();
}


void printBMP_rusEFI(){
  u8g2.clearBuffer();
  u8g2.drawXBM(0,0,dispWidth,dispHeight,BMP_rusEFI);
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0,0);
  //u8g2.print(BUILD);
  u8g2.sendBuffer();
}


void drawBarGraph(int locX, int locY, float current, float minimum, float maximum){

  // Draw a rectangle frame accross the display then fill with a bar
  // based on the current value and min/max
  
  float c = (((current-minimum)) / (maximum-minimum));
  if (c<0){
    c=0;
  }
  u8g2.drawFrame(locX,locY,(u8g2.getDisplayWidth()),10);
  int barWidth =  (c*(u8g2.getDisplayWidth()));
  u8g2.drawBox(locX,locY,barWidth,10);
 }

 void resetDisplay(){
    // Reset the OLED
    digitalWrite(OLED_RES, LOW);
    delay(500);
    digitalWrite(OLED_RES, HIGH);
    delay(500);
    u8g2.begin();
    reset = 1;
 }
