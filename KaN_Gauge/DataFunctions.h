/*
* ===================================================
* Define data values
* ====================================================
 */

#include "SensorData.h"

// Configure how test data increments
int lastTestIncrement = 0;
int testTime = 50;
float testIncrementValue = 1.01;

int Temperature = 0;
int Pressure =  0;
int Speed = 0;


//Pointer for current data
sensorData *ptrData;
sensorData *ptrData2;
sensorData *ptrData3;
sensorData *ptrData4;
sensorData *ptrDataLed1;
sensorData *ptrDataLed2;
sensorData *ptrDataLed4;

// ========================================================== Functions =========================================================================
void incrementTestData(){
  //Increment test data
  if ((lastTestIncrement+testTime) < millis()){
    lastTestIncrement = millis();
    test.scaledValue =  test.scaledValue + testIncrementValue;
    if (test.scaledValue > 110){
      test.scaledValue = 0;
    }
  }
}


void SAVE_DATA(CANMessage CANmsg) {

      // Check the ID of the recieved CAN message, scale and store the sensor values
      // Some data is sent in two bytes and is combined into a word before scaling
      
       switch(CANmsg.id){
          
          case(512):          
            warningCounter = ((int)word(CANmsg.data[1],CANmsg.data[0]));
            lastError = ((int)word(CANmsg.data[3],CANmsg.data[2]));
            revLimit = ((int)CANmsg.data[4]) & 0x01;
            mainRelay = ((int)CANmsg.data[4]) & 0x02;
            fuelPump = ((int)CANmsg.data[4]) & 0x04;
            checkEngine = ((int)CANmsg.data[4]) & 0x08;
            egoHeater = ((int)CANmsg.data[4]) & 0x10;
            break;
                    
          case(513):
            rpm.scaledValue = ((((int)word(CANmsg.data[1],CANmsg.data[0])) * (rpm.scaleMultiplier)) + rpm.offset);
            ignitionTiming.scaledValue = ((((float)word(CANmsg.data[3],CANmsg.data[2])) * (ignitionTiming.scaleMultiplier)) + ignitionTiming.offset);
            injectorDuty.scaledValue = ((((float)word(CANmsg.data[5],CANmsg.data[4])) * (injectorDuty.scaleMultiplier)) + injectorDuty.offset);
            vss.scaledValue = ((((float)(CANmsg.data[6])) * (vss.scaleMultiplier)) + vss.offset);
            vss_mph.scaledValue = ((((float)(CANmsg.data[6])) * (vss_mph.scaleMultiplier)) + vss_mph.offset);
                        
            break;

          case(514):
            accelerator.scaledValue = ((((int)word(CANmsg.data[1],CANmsg.data[0])) * (accelerator.scaleMultiplier)) + accelerator.offset);
            throttle1.scaledValue = ((((int)word(CANmsg.data[3],CANmsg.data[2])) * (throttle1.scaleMultiplier)) + throttle1.offset);
            throttle2.scaledValue = ((((int)word(CANmsg.data[5],CANmsg.data[4])) * (throttle2.scaleMultiplier)) + throttle2.offset);
            break;
            
          case(515):

            manifoldPressure.scaledValue = ((((float)word(CANmsg.data[1],CANmsg.data[0])) * (manifoldPressure.scaleMultiplier)) + manifoldPressure.offset);
            manifoldPressure_psi.scaledValue = ((((float)word(CANmsg.data[1],CANmsg.data[0])) * (manifoldPressure_psi.scaleMultiplier)) + manifoldPressure_psi.offset);      
            coolantTemperature.scaledValue = ((((float)(CANmsg.data[2])) * (coolantTemperature.scaleMultiplier)) + coolantTemperature.offset);
            coolantTemperature_f.scaledValue = ((((float)(CANmsg.data[2])) * (coolantTemperature_f.scaleMultiplier)) + coolantTemperature_f.offset);
            intakeTemperature.scaledValue = ((((float)(CANmsg.data[3])) * (intakeTemperature.scaleMultiplier)) + intakeTemperature.offset);
            intakeTemperature_f.scaledValue = ((((float)(CANmsg.data[3])) * (intakeTemperature_f.scaleMultiplier)) + intakeTemperature_f.offset);
            
            auxTemp1.scaledValue = ((((float)(CANmsg.data[4])) * (auxTemp1.scaleMultiplier)) + auxTemp1.offset);
            auxTemp2.scaledValue = ((((float)(CANmsg.data[5])) * (auxTemp2.scaleMultiplier)) + auxTemp2.offset);
            mcuTemp.scaledValue = ((((float)(CANmsg.data[6])) * (mcuTemp.scaleMultiplier)) + mcuTemp.offset);
            fuel.scaledValue = ((((float)(CANmsg.data[7])) * (fuel.scaleMultiplier)) + fuel.offset);
            break;
            
          case(516):
            afr.scaledValue = ((((float)word(CANmsg.data[1],CANmsg.data[0])) * (afr.scaleMultiplier)) + afr.offset);
            lambda.scaledValue = ((((float)word(CANmsg.data[1],CANmsg.data[0])) * (lambda.scaleMultiplier)) + lambda.offset);
            oilPressure_psi.scaledValue = ((((float)word(CANmsg.data[3],CANmsg.data[2])) * (oilPressure_psi.scaleMultiplier)) + oilPressure_psi.offset);
            oilPressure.scaledValue = ((((float)word(CANmsg.data[3],CANmsg.data[2])) * (oilPressure.scaleMultiplier)) + oilPressure.offset);
            vvtPosition.scaledValue = ((((float)word(CANmsg.data[5],CANmsg.data[4])) * (vvtPosition.scaleMultiplier)) + vvtPosition.offset);
            batteryVoltage.scaledValue = ((((float)word(CANmsg.data[7],CANmsg.data[6])) * (batteryVoltage.scaleMultiplier)) + batteryVoltage.offset);
            break;

          case(517):
            airMass.scaledValue = ((((float)word(CANmsg.data[1],CANmsg.data[0])) * (airMass.scaleMultiplier)) + airMass.offset);
            estimatedAirflow.scaledValue = ((((float)word(CANmsg.data[3],CANmsg.data[2])) * (estimatedAirflow.scaleMultiplier)) + estimatedAirflow.offset);
            injectorPulse.scaledValue = ((((float)word(CANmsg.data[5],CANmsg.data[4])) * (injectorPulse.scaleMultiplier)) + injectorPulse.offset);
            break;
            
      }
}
