
//Particle Functions
#include "Particle.h"
#include "take_measurements.h"
#include "device_pinout.h"
#include "MyPersistentData.h"

bool takeMeasurements() { 

    if (!batteryState()) sysStatus.set_lowPowerMode(true);

    isItSafeToCharge();

    if (Particle.connected()) getSignalStrength();

    sensorControl(true); // Turn the sensor so we can take a measurement

    delay(100);     // Let the sensor warm up

    current.set_distance(analogRead(DISTANCE_PIN)*0.00078125);  // 3.2V/4096 = 0.00078125V per bit

    sensorControl(false); // Turn the sensor off

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
