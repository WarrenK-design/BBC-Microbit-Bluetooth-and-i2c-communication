//Name:           Warren Kavanagh 
//Student Number: C16463344
//Date:           09/10/2019

//Description//
// Used to connect the BBC microbit to a phone/computer through bluetooth
// The services enabled are the accelerometer, the magnetometer, the LED and 
// button A 

//Useful Resources//
// MBED API                         - https://os.mbed.com/docs/mbed-os/v5.14/apis/index.html
// BLE Library                      - https://os.mbed.com/docs/mbed-os/v5.14/apis/ble.html
// GAP setup                        - https://os.mbed.com/docs/mbed-os/v5.14/apis/gap.html
// GATT Client                      - https://os.mbed.com/docs/mbed-os/v5.14/apis/gattclient.html
// GATT Server                      - https://os.mbed.com/docs/mbed-os/v5.14/apis/gattclient.html
// Magnetometer MAG3110             - https://www.nxp.com/docs/en/data-sheet/MAG3110.pdf
// Accelerometer MMA8653            - https://www.nxp.com/docs/en/data-sheet/MMA8653FC.pdf
// Microcontroller nRF51822         - https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF51822

#include "mbed.h"           //Mbed library 
#include "ble/BLE.h"        //Bluetooth low energy library
#include "LEDService.h"     //Handles the LED bluetooth Service and characteristsics 
#include "ButtonAService.h" //Handles the Button A bluetooth Service and characteristsics 
#include "accelService.h"   //Handles the Accelerometer bluetooth Service and characteristsics 
#include "magservice.h"     //Handles the Magnetometer bluetooth Service and characteristsics 


// The LED's which will illuminate:
// alivenessLED - Whne button A pressed 
// actuatedLED  - Write to LED over bluetooth
DigitalOut col(P0_4, 0);
DigitalOut alivenessLED(P0_13, 0);
DigitalOut actuatedLED(P0_14, 0);

// The device name that will appear to phone/computer when connecting through bluetooth
const static char     DEVICE_NAME[] = "WarrenBBC";

// uuid_list - array 16 bit integers, these will be the UUID (Universal Unique Identification Number) of each of the bluetooth enabled services 
static const uint16_t uuid16_list[] = {LEDService::LED_SERVICE_UUID,ACCELService::ACCEL_SERVICE_UUID,ButtonAService::BUTTONA_SERVICE_UUID,MAGService::MAG_SERVICE_UUID};
//static const uint16_t uuid16_list[] = {0xA012,0xFFF3};

// Pointers to the services 
// Can be used as references to call class functions
LEDService *ledServicePtr;
ButtonAService * btnAServicePtr;
ACCELService *AccelServicePtr;
MAGService * MagServicePtr;

// Ticker is used to genrate interrputs every set interval of time 
// ticker  - Used for polling interupt to poll BLE services
// ticker2 - Used for checking Accelrometer value to update direction on LED display
Ticker ticker;
Ticker ticker2;

/// disconnectionCallback ///
// This callback is associated with the ble object when the event of a dissconnect occurs
// If a dissconnect occurs this fuction will tell the GAP peripheral (BBC Microbit) to 
// begin adevertising again to GAP centrals (Phones/Computers)
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising();
}

/// periodicCallback ///
// This function is called every second through an intterupt genrated in the main()
// The function will poll the value of each of the services using the poll functions in
// the classes ACCELService, MAGService and ButtonAService which checks the value of each services charcteristics
// If button A is pressed it will turn on an LED 
void periodicCallback(void)
{
    btnAServicePtr->poll(); //polling checks all I/O for btn 
    AccelServicePtr->poll();//polling checks all I/O for Accel
    MagServicePtr->poll(); //polling checks all I/O for Mag
    //Turn on LED if btn push 
    if (btnAServicePtr->GetButtonAState()){
        alivenessLED =1;
        }
   else{
      alivenessLED=0;
        }       
}

//directionCallback//
// Function called every 0.1 secs in the main through intterupt 
// The function calls the Direction() function in the ACCELService class
// which will update the arrow direction on the LED display 
void directionCallback(){
    AccelServicePtr->Direction();
    }

 
 /// onDataWrittenCallback ///
 // This callback is used if the GATT server (BBC Microbit) recieves a write from the GATT client (PC/Phone)
 // The only write enabled service is the LED service, if a right is recieved an LED will be turned on 
void onDataWrittenCallback(const GattWriteCallbackParams *params) {
    if ((params->handle == ledServicePtr->getValueHandle()) && (params->len == 1)) {
        actuatedLED = *(params->data);
    }
}

void onDataReadCallback(const GattReadCallbackParams *params) {
  
}
/**
 * This function is called when the ble initialization process has failed
 */
 
void onBleInitError(BLE &ble, ble_error_t error)
{
    /* Initialization error handling should go here */
}


 /// bleInitComplete ///
 // This callback will be called when the default intiliasation of the ble object is complete
 // The purpose of this call back is that the the custom servcices must be associated with the ble object
 // before all of the intilisation process is complete. The ble object must also be told what action to take 
 // in certain evernts if an error occurs or disconnect occurs 
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    //BLE instance
    //ble_error_t instance
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;
    
    // Checks if any errors where dectected 
    // If there are errors forward the error handle to onBleInitError
    if (error != BLE_ERROR_NONE) {
        onBleInitError(ble, error);
        return;
    }
    
    // Ensure that it is the default instance of BLE 
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }
    
    // If the ble device is disconnected go to the disconnectionCallback function 
    // This will handle the disconnection aprropiately and take appropiate action 
    ble.gap().onDisconnection(disconnectionCallback);
    
    // If the GATT Server (BBC Micobit) recieves a write from the GATT Client (phone/computer)
    // the onDataWritten function is used too tell the GATT server how to handle this 
    // In this case it goes to the callback onDataWrittenCallback
    ble.gattServer().onDataWritten(onDataWrittenCallback);
    
    // ble.gattServer().onDataRead(onDataReadCallback); // Nordic Soft device will not call this so have to poll instead

    // The LED's intial state can be configuered to logic high or logic low 
    bool initialValueForLEDCharacteristic = false;
    
    // Creates the LED service object passing the ble object and the intial state of LED 
    ledServicePtr = new LEDService(ble, initialValueForLEDCharacteristic);
    
    // Creates the button service passing the ble object to the constructor 
    btnAServicePtr = new ButtonAService(ble);
    
    //Intial value is used for the starting value for the X,Y and Z plane of the magnetometer and accelerometer 
    int16_t InitialValue=0;
    
    // Creates the Acclerometter service intialising instance of the ACCELService class passing the ble object and intial value  
    AccelServicePtr = new ACCELService(ble,InitialValue);
    
    // Creates the Magnetometer service intialising instance of the MAGService class passing the ble object and intial value
    MagServicePtr = new MAGService(ble,InitialValue);
    
    // The Generic access profile (GAP) portion of the code 
    // After the services have been set up and associated with the ble object they can be advertised
    // Remeber Gap splits the devices being connected into a peripheral and central 
    // The BBC microbit will be our peripheral device so it must adverstise to the central what services it has 
    // Gap is used to adverstise the Gap peripheral (BBC Microbit) to the gap central (phone/computer)
    // MBED's documentation on GAP api https://os.mbed.com/docs/mbed-os/v5.14/apis/gap.html
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
    ble.gap().startAdvertising();
}

///main///
// main body
int main(void)
{
    // Ticker object is used to set up an innterupt
    // ticker  - The intterupt calls the function periodicCallback which polls each of the services 
    // ticker2 - The interupt to update the arrow on the LED display 
    ticker.attach(periodicCallback, 1);
    ticker2.attach(directionCallback, 0.1);

    //Get software object that reprensts BLE on BBC
    BLE &ble = BLE::Instance();
    
    // Once the intialisation is complete a callback function, bleInitComplete will be called
    // The inistialisation depends on setting up the custom services and characteristics for the accelerometer, magnetometer, button and LED's  
    // The bluetooth device can then begin advertising 
    ble.init(bleInitComplete);

    // Waiting for the BLE object to finish intialising  
    while (ble.hasInitialized()  == false) { /* spin loop */ }
    
    //BLE object has succesfully intalised Succesful 
    while (true) {
        ble.waitForEvent();
    }
}
