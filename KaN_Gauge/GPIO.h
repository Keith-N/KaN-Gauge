#define PCBREV_1.0
#ifdef PCBREV_1.0

  #define BOOT      0
  #define UART_TX   1
  #define UART_RX   3

  // JTAG
  #define MTDI  12
  #define MTCK  13
  #define MTMS  14
  #define MTDO  15

  // CAN
  #define CAN_RX  GPIO_NUM_17
  #define CAN_TX  GPIO_NUM_18

  // I2C
  #define I2C_SDA 22
  #define I2C_SCL 23

  #define OLED_RES 5

  // Inputs
  #define USER_INPUT 16
  #define USER_INPUT2 4
  
  #define SPARE_1  36 
  #define SPARE_2  39   

  // LEDs
  //#define LED_0     
  #define LED_1 33    
  #define LED_2 32    
  #define LED_3 14     
  #define LED_4 12   
  #define LED_5 25    
  #define LED_6 26    
  #define LED_7 27    
  #define LED_8 13    
  #define LED_9 15    
  #define LED_10 2
  #define LED_11 21
  #define LED_12 19

 
void PIN_SETUP(){
  //Setup input button and interrupt
  pinMode(USER_INPUT2, INPUT_PULLUP);
  pinMode(USER_INPUT, INPUT_PULLUP);
  //pinMode(SPARE_1, INPUT_PULLUP);
  
  pinMode(OLED_RES, OUTPUT);
  digitalWrite(OLED_RES,HIGH);

  //Solder jumper
  pinMode(SPARE_1, INPUT);
  pinMode(SPARE_2, INPUT);
  
  // LED 
  //pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_5, OUTPUT);
  pinMode(LED_6, OUTPUT);
  pinMode(LED_7, OUTPUT);
  pinMode(LED_8, OUTPUT);
  pinMode(LED_9, OUTPUT);
  pinMode(LED_10, OUTPUT);
  pinMode(LED_11, OUTPUT);
  pinMode(LED_12, OUTPUT);
  
}
      
#endif


// ---------------------------------- LEDs ------------------------------------------------------

int indicatorLed[] = { LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7, LED_8, LED_9, LED_10};
int statusLed[] = {LED_11, LED_12};
int allLed[] = { LED_11, LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7, LED_8, LED_9, LED_10, LED_12};


void ledOn(){
 for (int i = 0; i<10; i++){
   digitalWrite(indicatorLed[i],HIGH);
   }
}

void ledOff(){
      for (int i = 0; i<12; i++){
        digitalWrite(allLed[i],LOW);
      };
}

void blinkLed(int wait){
  ledOn();  
  delay(wait);
  ledOff();
}

// Illuminate LEDs sequentially 0-10
void sequentialLed(int p){
  
  p = p/10;
  
  for (int i=0; i<10; i++){
     digitalWrite(indicatorLed[i],LOW);
   }

 #ifdef LED_SINGLE
   digitalWrite(indicatorLed[p],HIGH);
 #else

  for (int i = 0; i<p; i++){
    digitalWrite(indicatorLed[i],HIGH);
    }
 #endif
    

}

void sequentialLedAll(int p){
  
  p = p*12/100;
      
  for (int i=0; i<12; i++){
    digitalWrite(allLed[i],LOW);
    }
      
  #ifdef LED_SINGLE
     digitalWrite(allLed[p],HIGH);
  #else

  for (int i = 0; i<p; i++){
    digitalWrite(allLed[i],HIGH);
    }
  #endif

  #ifdef SERIAL_DEBUG
    Serial.print("LEDs ON : ");
    Serial.println("p");
  #endif
 
}

void toggleLeftLed(){
  digitalWrite(LED_11, !digitalRead(LED_11));
}

void toggleRightLed(){
  digitalWrite(LED_12, !digitalRead(LED_12));
}

void ledSweep( int startLed, int endLed, int t, int t2){

  for (int i = startLed; i<endLed; i++){
    digitalWrite(allLed[i],HIGH);
    delay(t);
    }

  for (int i = 0; i<12;i++){
    digitalWrite(allLed[i],LOW);
    delay(t2);
    }
 }
