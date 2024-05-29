
//Particle Functions
#include "Particle.h"
#include "take_measurements.h"
#include "device_pinout.h"
#include "MyPersistentData.h"
#include "math.h"

bool takeMeasurements() { 

    if (!batteryState()) sysStatus.set_lowPowerMode(true);

    isItSafeToCharge();

    if (Particle.connected()) getSignalStrength();

    /* Temperature Compensation
    Datasheet: https://maxbotix.com/pages/xl-maxsonar-wr-datasheet
    Using this guide:  https://maxbotix.com/pages/temperature-compensation-pdf

    We will first get the temperature of the sensor and then use the following formula to calculate the temperature compensated distance:
    Temperature Compensated Distance = Measured Distance + (Measured Distance * (0.000394 * (Temperature - 25)));
    Our sensor is the MB7092 XL-MaxSonar-WR1 and has a temperature range of -40 to +65 degrees C.

    Temperature Compensation when using the Analog Output on the Long Range XL-MaxSonar products (that output a voltage of one bit per two cm)
    DM = (Vm/(Vcc/2048)*(29e-6uS)) * (20.05*SQRT(Tc+273.15)/2)  - changed from datasheet as the Boron's ADC is 12 bit

    where:
      Tc is the temperature in degrees C
      Vm is the analog voltage output from our product (measured by the user)
      Vcc is the supply voltage powering the MaxSonar product and
      Dm is the distance in meters.
    */

    // Take the outside temperarture reading
    current.set_externalTempC((analogRead(EXTERNAL_TEMP_PIN) * 3.3 / 4096.0 - 0.5) * 100.0);  // 10mV/degC, 0.5V @ 0degC
    Log.info("External Temp: %4.2fC",current.get_externalTempC());

    // Take the distance reading - compensating for temperature
    current.set_distance(((analogRead(DISTANCE_PIN)*(3.3/4096)/(3.3/2048)*(29e-6)) * (20.05*sqrt(current.get_externalTempC()+273.15)/2))*100.0);      // Distance in cm
    Log.info("Distance: %dcm",current.get_distance());

    current.set_internalTempC((analogRead(INTERNAL_TEMP_PIN) * 3.3 / 4096.0 - 0.5) * 100.0);  // 10mV/degC, 0.5V @ 0degC
    Log.info("Internal Temp: %4.2fC",current.get_internalTempC());

    return 1;
}


bool batteryState() {

  current.set_batteryState(System.batteryState());                      // Call before isItSafeToCharge() as it may overwrite the context
  current.set_stateOfCharge(System.batteryCharge());                   // Assign to system value
  Log.info("Battery state of charge %4.2f%%",System.batteryCharge());

  if (current.get_stateOfCharge() > 60 || current.get_stateOfCharge() == -1 ) return true;  // Bad battery reading should not put device in low power mode
  else return false;
}


bool isItSafeToCharge()                             // Returns a true or false if the battery is in a safe charging range.
{
  PMIC pmic(true);
  if (current.get_internalTempC() < 0 || current.get_internalTempC() > 37 )  {  // Reference: (32 to 113 but with safety)
    pmic.disableCharging();                         // It is too cold or too hot to safely charge the battery
    current.set_batteryState(1);                       // Overwrites the values from the batteryState API to reflect that we are "Not Charging"
    return false;
  }
  else {
    pmic.enableCharging();                          // It is safe to charge the battery
    return true;
  }
}


void getSignalStrength() {
  char signalStr[16];
  const char* radioTech[10] = {"Unknown","None","WiFi","GSM","UMTS","CDMA","LTE","IEEE802154","LTE_CAT_M1","LTE_CAT_NB1"};
  // New Signal Strength capability - https://community.particle.io/t/boron-lte-and-cellular-rssi-funny-values/45299/8
  CellularSignal sig = Cellular.RSSI();

  auto rat = sig.getAccessTechnology();

  //float strengthVal = sig.getStrengthValue();
  float strengthPercentage = sig.getStrength();

  //float qualityVal = sig.getQualityValue();
  float qualityPercentage = sig.getQuality();

  snprintf(signalStr,sizeof(signalStr), "%s S:%2.0f%%, Q:%2.0f%% ", radioTech[rat], strengthPercentage, qualityPercentage);
  Log.info(signalStr);
}

float getTemperature(int reading) {                                     // Get temperature and make sure we are not getting a spurrious value

  if ((reading < 0) || (reading > 2048)) {                              // This corresponds to -50 degrees to boiling - outside this range we have an error
    return -255;
  }

  float voltage = reading * 3.3;                                      // converting that reading to voltage, for 3.3v arduino use 3.3
  voltage /= 4096.0;                                                  // Electron is different than the Arduino where there are only 1024 steps
  return ((voltage - 0.5) * 100.0);                    //converting from 10 mv per degree with 500 mV offset to degrees ((voltage - 500mV) times 100) - 5 degree calibration
}
