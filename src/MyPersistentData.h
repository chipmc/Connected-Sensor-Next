#ifndef __MYPERSISTENTDATA_H
#define __MYPERSISTENTDATA_H

#include "Particle.h"
#include "StorageHelperRK.h"

//Define external class instances. These are typically declared public in the main .CPP. I wonder if we can only declare it here?
// extern MB85RC64 fram;

//Macros(#define) to swap out during pre-processing (use sparingly). This is typically used outside of this .H and .CPP file within the main .CPP file or other .CPP files that reference this header file. 
// This way you can do "data.setup()" instead of "MyPersistentData::instance().setup()" as an example
#define current currentStatusData::instance()
#define sysStatus sysStatusData::instance()

/**
 * This class is a singleton; you do not create one as a global, on the stack, or with new.
 * 
 * From global application setup you must call:
 * MyPersistentData::instance().setup();
 * 
 * From global application loop you must call:
 * MyPersistentData::instance().loop();
 */

// *******************  SysStatus Storage Object **********************
//
// ********************************************************************

class sysStatusData : public StorageHelperRK::PersistentDataFile {
public:

    /**
     * @brief Gets the singleton instance of this class, allocating it if necessary
     * 
     * Use MyPersistentData::instance() to instantiate the singleton.
     */
    static sysStatusData &instance();

    /**
     * @brief Perform setup operations; call this from global application setup()
     * 
     * You typically use MyPersistentData::instance().setup();
     */
    void setup();

    /**
     * @brief Perform application loop operations; call this from global application loop()
     * 
     * You typically use MyPersistentData::instance().loop();
     */
    void loop();

	/**
	 * @brief Validates values and, if valid, checks that data is in the correct range.
	 * 
	 */
	bool validate(size_t dataSize);

	/**
	 * @brief Will reinitialize data if it is found not to be valid
	 * 
	 * Be careful doing this, because when MyData is extended to add new fields,
	 * the initialize method is not called! This is only called when first
	 * initialized.
	 * 
	 */
	void initialize();


	class SysData {
	public:
		// This structure must always begin with the header (16 bytes)
		StorageHelperRK::PersistentDataBase::SavedDataHeader sysHeader;
		// Your fields go here. Once you've added a field you cannot add fields
		// (except at the end), insert fields, remove fields, change size of a field.
		// Doing so will cause the data to be corrupted!
		uint8_t structuresVersion;                        // Version of the data structures (system and data)
		bool verboseMode;                                 // Turns on extra messaging
		bool solarPowerMode;                              // Powered by a solar panel or utility power
  		bool lowPowerMode;                                // Does the device need to run disconnected to save battery
		bool lowBatteryMode;                              // Is the battery level so low that we can no longer connect
		uint8_t resetCount;                               // reset count of device (0-256)
		char timeZoneStr[39];							  // String for the timezone - https://developer.ibm.com/technologies/systems/articles/au-aix-posix/
		uint8_t openTime;                                 // Hour the park opens (0-23)
		uint8_t closeTime;                                // Hour the park closes (0-23)
		time_t lastReport;								  // The last time we sent a webhook to the queue
		time_t lastConnection;                     		  // Last time we successfully connected to Particle
		time_t lastHookResponse;                   		  // Last time we got a valid Webhook response
		uint16_t lastConnectionDuration;                  // How long - in seconds - did it take to last connect to the Particle cloud
		uint8_t sensorType;                               // What is the sensor type - 0-Pressure Sensor, 1-PIR Sensor
		bool verizonSIM;                                  // Are we using a Verizon SIM?
	};

	SysData sysData;

	// 	******************* Get and Set Functions for each variable in the storage object ***********
    
	/**
	 * @brief For the Get functions, used to retrieve the value of the variable
	 * 
	 * @details Specific to the location in the object and the type of the variable
	 * 
	 * @param Nothing needed
	 * 
	 * @returns The value of the variable in the corret type
	 * 
	 */

	/**
	 * @brief For the Set functions, used to set the value of the variable
	 * 
	 * @details Specific to the location in the object and the type of the variable
	 * 
	 * @param Value to set the variable - correct type - for Strings they are truncated if too long
	 * 
	 * @returns None needed
	 * 
	 */

	uint8_t get_structuresVersion() const ;
	void set_structuresVersion(uint8_t value);

	bool get_verboseMode() const;
	void set_verboseMode(bool value);

	bool get_solarPowerMode() const;
	void set_solarPowerMode(bool value);

	bool get_lowPowerMode() const;
	void set_lowPowerMode(bool value);

	bool get_lowBatteryMode() const;
	void set_lowBatteryMode(bool value);

	uint8_t get_resetCount() const;
	void set_resetCount(uint8_t value);

	String get_timeZoneStr() const;
	bool set_timeZoneStr(const char *str);

	uint8_t get_openTime() const;
	void set_openTime(uint8_t value);

	uint8_t get_closeTime() const;
	void set_closeTime(uint8_t value);

	time_t get_lastReport() const;
	void set_lastReport(time_t value);

	time_t get_lastConnection() const;
	void set_lastConnection(time_t value);

	uint16_t get_lastConnectionDuration() const;
	void set_lastConnectionDuration(uint16_t value);

	time_t get_lastHookResponse() const;
	void set_lastHookResponse(time_t value);

	uint8_t get_alertCodeNode() const;
	void set_alertCodeNode(uint8_t value);

	time_t get_alertTimestampNode() const;
	void set_alertTimestampNode(time_t value);

	uint8_t get_sensorType() const;
	void set_sensorType(uint8_t value);

	bool get_verizonSIM() const;
	void set_verizonSIM(bool value);


	//Members here are internal only and therefore protected
protected:
    /**
     * @brief The constructor is protected because the class is a singleton
     * 
     * Use MyPersistentData::instance() to instantiate the singleton.
     */
    sysStatusData();

    /**
     * @brief The destructor is protected because the class is a singleton and cannot be deleted
     */
    virtual ~sysStatusData();

    /**
     * This class is a singleton and cannot be copied
     */
    sysStatusData(const sysStatusData&) = delete;

    /**
     * This class is a singleton and cannot be copied
     */
    sysStatusData& operator=(const sysStatusData&) = delete;

    /**
     * @brief Singleton instance of this class
     * 
     * The object pointer to this class is stored here. It's NULL at system boot.
     */
    static sysStatusData *_instance;

    //Since these variables are only used internally - They can be private. 
	static const uint32_t SYS_DATA_MAGIC = 0x20a99e75;
	static const uint16_t SYS_DATA_VERSION = 2;

};




// *****************  Current Status Storage Object *******************
//
// ********************************************************************

class currentStatusData : public StorageHelperRK::PersistentDataFile {
public:

    /**
     * @brief Gets the singleton instance of this class, allocating it if necessary
     * 
     * Use MyPersistentData::instance() to instantiate the singleton.
     */
    static currentStatusData &instance();

    /**
     * @brief Perform setup operations; call this from global application setup()
     * 
     * You typically use MyPersistentData::instance().setup();
     */
    void setup();

    /**
     * @brief Perform application loop operations; call this from global application loop()
     * 
     * You typically use MyPersistentData::instance().loop();
     */
    void loop();

	/**
	 * @brief Load the appropriate system defaults - good ot initialize a system to "factory settings"
	 * 
	 */
	void loadCurrentDefaults();                          // Initilize the object values for new deployments

	/**
	 * @brief Resets the current and hourly counts
	 * 
	 */
	void resetEverything();  

	/**
	 * @brief Validates values and, if valid, checks that data is in the correct range.
	 * 
	 */
	bool validate(size_t dataSize);

	/**
	 * @brief Will reinitialize data if it is found not to be valid
	 * 
	 * Be careful doing this, because when MyData is extended to add new fields,
	 * the initialize method is not called! This is only called when first
	 * initialized.
	 * 
	 */
	void initialize();  

	class CurrentData {
	public:
		// This structure must always begin with the header (16 bytes)
		StorageHelperRK::PersistentDataBase::SavedDataHeader currentHeader;
		// Your fields go here. Once you've added a field you cannot add fields
		// (except at the end), insert fields, remove fields, change size of a field.
		// Doing so will cause the data to be corrupted!
		// You may want to keep a version number in your data.
		uint16_t distance;                             // distance in cm
		time_t lastCountTime;
		float internalTempC;                            // Enclosure temperature in degrees C
		float externalTempC;							// Temp Sensor at the ultrasonic device
		uint8_t alertCode;								// Current Alert Code
		time_t lastAlertTime;
		float stateOfCharge;                                // Battery charge level
		uint8_t batteryState;                             // Stores the current battery state
	};
	CurrentData currentData;

	// 	******************* Get and Set Functions for each variable in the storage object ***********
    
	/**
	 * @brief For the Get functions, used to retrieve the value of the variable
	 * 
	 * @details Specific to the location in the object and the type of the variable
	 * 
	 * @param Nothing needed
	 * 
	 * @returns The value of the variable in the corret type
	 * 
	 */

	/**
	 * @brief For the Set functions, used to set the value of the variable
	 * 
	 * @details Specific to the location in the object and the type of the variable
	 * 
	 * @param Value to set the variable - correct type - for Strings they are truncated if too long
	 * 
	 * @returns None needed
	 * 
	 */

	uint16_t get_distance() const;
	void set_distance(uint16_t value);

	time_t get_lastCountTime() const;
	void set_lastCountTime(time_t value);

	float get_internalTempC() const ;
	void set_internalTempC(float value);

	float get_externalTempC() const ;
	void set_externalTempC(float value);

	int8_t get_alertCode() const;
	void set_alertCode(int8_t value);

	time_t get_lastAlertTime() const;
	void set_lastAlertTime(time_t value);

	float get_stateOfCharge() const;
	void set_stateOfCharge(float value);

	uint8_t get_batteryState() const;
	void set_batteryState(uint8_t value);


		//Members here are internal only and therefore protected
protected:
    /**
     * @brief The constructor is protected because the class is a singleton
     * 
     * Use MyPersistentData::instance() to instantiate the singleton.
     */
    currentStatusData();

    /**
     * @brief The destructor is protected because the class is a singleton and cannot be deleted
     */
    virtual ~currentStatusData();

    /**
     * This class is a singleton and cannot be copied
     */
    currentStatusData(const currentStatusData&) = delete;

    /**
     * This class is a singleton and cannot be copied
     */
    currentStatusData& operator=(const currentStatusData&) = delete;

    /**
     * @brief Singleton instance of this class
     * 
     * The object pointer to this class is stored here. It's NULL at system boot.
     */
    static currentStatusData *_instance;

    //Since these variables are only used internally - They can be private. 
	static const uint32_t CURRENT_DATA_MAGIC = 0x20a99e74;
	static const uint16_t CURRENT_DATA_VERSION = 2;
};


#endif  /* __MYPERSISTENTDATA_H */
