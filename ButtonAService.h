#ifndef __BUTTONA_SERVICE_H__
#define __BUTTONA_SERVICE_H__
// Button A is on P0 bit 17 
volatile uint32_t * P0OUT = (uint32_t *)0x50000504;
volatile uint32_t * P0DIR = (uint32_t *)0x50000514;
volatile uint32_t * P0IN  = (uint32_t *)0x50000510;
volatile uint32_t * P0CONF  = (uint32_t *)(0x50000700);

//Initial value of button is 0
int8_t initialValue=0;

///ButtonAService///
// Contains all the functions and class variables associated with Button A
// Creates the Button A service in the BLE profile 
// Reads the state characteristic for the Button A service 
class ButtonAService {
public:
    // UUID - Universal unique identification number assigned to the Button A of 0x1eee 
    const static uint16_t BUTTONA_SERVICE_UUID              = 0x1eee;
    // The chracteristic of the state of the button also requires a UUID, assigned 0x2019
    const static uint16_t BUTTONA_STATE_CHARACTERISTIC_UUID = 0x2019;

    ///ButtonAService Constructor///
    // Will create the Button A service for bluetooth profile 
    // Assigns the UUID's decalred for Button A and Characteristics 
    ButtonAService(BLEDevice &_ble) :
        ble(_ble), ButtonState(BUTTONA_STATE_CHARACTERISTIC_UUID,&initialValue,GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)
    {
        // Assign the gatt characteristics to a GattCharacteristic instance 
        GattCharacteristic *charTable[] = {&ButtonState};
        // Create an instance of a service for Button A and associate the characteristic for state with it
        GattService         btnService(BUTTONA_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        // Add the service to the ble profile 
        ble.addService(btnService);
        
        // On power up, input buffer is not connected so must do this
        P0CONF[17] = 0;  
        PreviousButtonState = -1;
    }
    
    // Get the value of ButtonState handle 
    GattAttribute::Handle_t getValueHandle() const {
        return ButtonState.getValueHandle();
    }
    
    /// poll ///
    // Checks if the button has been clicked by comparing the previous value
    // with the current value of the button and only sends an update if button state changes 
    void poll() {
        // First get the state of the button 
        uint8_t newValue = GetButtonAState();
        
        //Only notify if there is a new button state 
        if (newValue != PreviousButtonState) 
        {   
            // only send an update if the button state has changed (reduces traffic)
            ble.gattServer().write(this->getValueHandle(), (uint8_t *)&newValue, sizeof(uint8_t));        
            PreviousButtonState = newValue;
        }
    }
    /// GetButtonAState ///
    // Returns the state of button A by setting up a mask for pin 17 
    // It thens AND's this mask with pin 17 to see if result of AND in a 1 or 0
    uint8_t GetButtonAState()
    {
        if ((*P0IN & (1 << 17))==0)    
            return 1;
        else
            return 0;
    }

//Private variables
private:
    BLEDevice  &ble;
    ReadOnlyGattCharacteristic<int8_t>  ButtonState;
    uint8_t PreviousButtonState;
    
};

#endif
