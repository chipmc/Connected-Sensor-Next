//Particle Functions
#include "Particle.h"
#include "device_pinout.h"

/**********************************************************************************************************************
 * ******************************         Boron Pinout Example               ******************************************
 * https://docs.particle.io/reference/datasheets/b-series/boron-datasheet/#pins-and-button-definitions
 *
 Left Side (16 pins)
 * !RESET -
 * 3.3V -
 * !MODE -
 * GND -
 * D19 - A0 -               
 * D18 - A1 -               DISTANCE_PIN - Ultrasonic Sensor Pin      
 * D17 - A2 -               MODULE_POWER_PIN - Bringing this high powers up the PIR sensor          
 * D16 - A3 -               LED_POWER_PIN - Enables or disables the LED on the PIR module
 * D15 - A4 -               Internal (TMP32) Temp Sensor
 * D14 - A5 / SPI SS -      
 * D13 - SCK - SPI Clock
 * D12 - MO - SPI MOSI
 * D11 - MI - SPI MISO 
 * D10 - UART RX -
 * D9 - UART TX -

 Right Size (12 pins)
 * Li+
 * ENABLE
 * VUSB -
 * D8 -                     Wake Connected to Watchdog Timer
 * D7 -                     Blue Led
 * D6 -                     
 * D5 -                     
 * D4 -                     User Switch
 * D3 - 
 * D2 -                     
 * D1 - SCL - I2C Clock -   FRAM / RTC and I2C Bus
 * D0 - SDA - I2C Data -    FRAM / RTX and I2C Bus
***********************************************************************************************************************/

// Carrier Board standard pins
const pin_t INTERNAL_TEMP_PIN   = A4;
const pin_t BUTTON_PIN        = D4;
const pin_t BLUE_LED          = D7;
const pin_t WAKEUP_PIN        = D8;

// Sensor specific Pins
extern const pin_t DISTANCE_PIN = A1;                   // May need to change this
extern const pin_t EXTERNAL_TEMP_PIN = A2;          // External temp sensor
const pin_t LED_POWER_PIN = A3;                    // Not use for this sketch

bool initializePinModes() {
    Log.info("Initalizing the pinModes");
    // Define as inputs or outputs
    pinMode(BUTTON_PIN,INPUT_PULLUP);               // User button on the carrier board - active LOW
    pinMode(WAKEUP_PIN,INPUT);                      // This pin is active HIGH
    pinMode(BLUE_LED,OUTPUT);                       // On the Boron itself
    pinMode(DISTANCE_PIN, INPUT);
    pinMode(EXTERNAL_TEMP_PIN, INPUT);
    pinMode(LED_POWER_PIN,OUTPUT);

    return true;
}

bool initializePowerCfg() {
    Log.info("Initializing Power Config");
    const int maxCurrentFromPanel = 900;            // Not currently used (100,150,500,900,1200,2000 - will pick closest) (550mA for 3.5W Panel, 340 for 2W panel)
    SystemPowerConfiguration conf;
    System.setPowerConfiguration(SystemPowerConfiguration());  // To restore the default configuration

    conf.feature(SystemPowerFeature::PMIC_DETECTION)
        .powerSourceMaxCurrent(maxCurrentFromPanel) // Set maximum current the power source can provide  3.5W Panel (applies only when powered through VIN)
        .powerSourceMinVoltage(5080) // Set minimum voltage the power source can provide (applies only when powered through VIN)
        .batteryChargeCurrent(maxCurrentFromPanel) // Set battery charge current
        .batteryChargeVoltage(4208) // Set battery termination voltage
        .feature(SystemPowerFeature::USE_VIN_SETTINGS_WITH_USB_HOST); // For the cases where the device is powered through VIN
                                                                     // but the USB cable is connected to a USB host, this feature flag
                                                                     // enforces the voltage/current limits specified in the configuration
                                                                     // (where by default the device would be thinking that it's powered by the USB Host)
    int res = System.setPowerConfiguration(conf); // returns SYSTEM_ERROR_NONE (0) in case of success
    if (res == 0) Log.info("Power configuration process succcesful");
    return res;
}
                
