
#ifndef __BLE_LED_SERVICE_H__
#define __BLE_LED_SERVICE_H__

///LEDService///
// Contains all the functions and class variables associated with LED
// Creates the LED service in the BLE profile 
// Reads the state characteristic for the LED service 
class LEDService {
public:
    //Universal Unique Identification numbers for LED//
    //The LED service has a UUID of 0xA000
    //THe characteristic for the state of the LED has UUID of 
    const static uint16_t LED_SERVICE_UUID              = 0xA000; //What you see on BLE 
    const static uint16_t LED_STATE_CHARACTERISTIC_UUID = 0xA001;


    ///LEDService Constructor///
    // Will create the LED service for bluetooth profile 
    // Assigns the UUID's decalred for LED and Characteristics 
    LEDService(BLEDevice &_ble, bool initialValueForLEDCharacteristic) :
        ble(_ble), ledState(LED_STATE_CHARACTERISTIC_UUID, &initialValueForLEDCharacteristic)
    {
        // Assign the gatt characteristics to a GattCharacteristic instance
        GattCharacteristic *charTable[] = {&ledState};
        // Create an instance of a service for the LED and associate the characteristics with it
        GattService         ledService(LED_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        // Add the service to the ble profile 
        ble.addService(ledService); ///Returns your handle for LED 
    }
    
    //Gets value of LED state 
    GattAttribute::Handle_t getValueHandle() const {
        return ledState.getValueHandle();
    }

//Private variables 
private:
    BLEDevice                         &ble;
    ReadWriteGattCharacteristic<bool>  ledState; //<> are templates in C++
};

#endif /* #ifndef __BLE_LED_SERVICE_H__ */
