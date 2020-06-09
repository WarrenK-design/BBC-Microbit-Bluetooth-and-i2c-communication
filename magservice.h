//Magnetometer: MAG3110
//Datasheet:    https://www.nxp.com/docs/en/data-sheet/MAG3110.pdf
#ifndef __BLE_MAG_SERVICE_H__
#define __BLE_MAG_SERVICE_H__
#include <mbed.h>

// The standard i2c slave address for MAG3110 is 0x0e
const int MAG3110_ADDRESS = (0x0e<<1);

///MAGService///
// Contains all the functions and class variables associated with Magnetometer
// Creates the Magnetometer service in the BLE profile 
// Reads the X,Y and Z characteristics for the Magnetometer service 
class MAGService {
public:
    // UUID - Universal unique identification number assigned to the Magnerometer of 0xfff3 
    const static uint16_t MAG_SERVICE_UUID = 0xfff3;
    
    // The magnontometer service has 3 characetersistics 
    // Each charactersistic is assigned a UUID 
    // The magnectic field strenght in the X, Y and Z plane are charcteristics 
    // UUID X plane characteristic - 0x01
    // UUID Y plane characteristic - 0x02
    // UUID Z plane characteristic - 0x03
    const static uint16_t MAG_X_CHARACTERISTIC_UUID = 0x1;
    const static uint16_t MAG_Y_CHARACTERISTIC_UUID = 0x2;
    const static uint16_t MAG_Z_CHARACTERISTIC_UUID = 0x3;
     
     //MAGService Constructor//
    // Will create the magnontometer service for bluetooth profile 
    // Assigns the UUID's decalred for magnontometer and Characteristics 
    // Wakes up the magnontometer by setting bit 7 in CNTRL_REG_2 and setting bit 0 in CNTRL_REG_A 
    MAGService(BLEDevice &_ble, int16_t initialValueForMAGCharacteristic) :
        ble(_ble), MagX(MAG_X_CHARACTERISTIC_UUID, &initialValueForMAGCharacteristic),MagY(MAG_Y_CHARACTERISTIC_UUID, &initialValueForMAGCharacteristic),MagZ(MAG_Z_CHARACTERISTIC_UUID, &initialValueForMAGCharacteristic)
    {
        // Assign the gatt characteristics to a GattCharacteristic instance 
        GattCharacteristic *charTable[] = {&MagX,&MagY,&MagZ};
        // Create an instance of a service for the magnetometer and associate the characteristics with it
        GattService         MagService(MAG_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        // Add the service to the ble profile 
        ble.addService(MagService);
        
        // Wake the accelerometer from sleep mode 
        // Step 1. Set bit 7 in CTRL_REG2 by writing 0x80 to register number 0x11   
        char Data[8]; // Declare a buffer for data transfer    
        int Status;
        Data[0]=0x11;//Control Reg 2
        Data[1]=0x80;
        Status = i2c.write(MAG3110_ADDRESS,Data,2);    
        
        // Step 2. Set bit 0 in CNTRL_REGA by writing 1 to register number 0x01
        Data[0]=0x10; //Control Regsiter A 
        Data[1]=1;
        Status = i2c.write(MAG3110_ADDRESS,Data,2);  // Write data to register 
    }

    GattAttribute::Handle_t getValueHandle() const {
        return MagX.getValueHandle();
    }
    
    // Updates the value of the X characteristic 
    void updateMagX(int16_t newValue) {
        ble.gattServer().write(MagX.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));    
    }
    // Updates the value of the Y characteristic 
    void updateMagY(int16_t newValue) {
        ble.gattServer().write(MagY.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }
    // Updates the value of the Z characteristic 
    void updateMagZ(int16_t newValue) {
        ble.gattServer().write(MagZ.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }
    
     ///poll///
    //Poll will get the value of the magnontometer for the x, y and z planes
    //The values of the x, y and z planes are updated using i2c 
    //The two functions used in i2c are i2c.write and i2c.read
    
    //i2c.write - The master writes to the slave using i2c, requires 4 parameters 
    //Address   - The i2c device address that will be written to, the i2c address is in the variable MMA8653_ADDRESS 0x1D
    //Data      - The internal register to write, remeber a write of the memory location that is to be read is required first in i2c  
    //lenght    - The number of bytes to send, in the case below 1 byte will be written at a time the registers for the MSB's of the X, Y and Z planes
    //repeated  - Default value is False,set True does not send stop condition at end of write, this means multiple i2c transactions can happen sequentially all teminated by 1 stop condition 
      
    //i2c.read  - Performs an i2c read transaction, requires 4 parameters 
    //Address   - The i2c address of the devce that will be read, here MMA8653_ADDRESS 0x1D
    //Data      - The internal register that's value will be read, in the MMA8653 it will be the X, Y and Z registers for accleration
    //lenght    - The number of bytes to read, if one byte adress is given and lenght set to 2 bytes it will return the next value of next memory location along with value of original memeory location requested 
    //repeated  - Default value is False, set True does not send a stop condition at end of read, this means multiple i2c transactions can happen sequentially all teminated by 1 stop condition 
    void poll()
    {
        char Data[8]; // Declare a buffer for data transfer    
        int Status;
        int16_t X;
        
        Data[0]=0x01;                                     // Register number 1 X_MSB
        Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MAG3110_ADDRESS,Data,2);        // Read register contents
        X = (Data[0]<<8)+Data[1];                         // 2 bytes returned, shift by 8 bits and AND Least signficant byte to get 16 bit number 
        
        int16_t Y;
        Data[0]=0x03;                                     // Register number 3 Y_MSB
        Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MAG3110_ADDRESS,Data,2);        // Read register contents
        Y = (Data[0]<<8)+Data[1];                         // 2 bytes returned, shift by 8 bits and AND Least signficant byte to get 16 bit number 
        
        int16_t Z;
        Data[0]=0x05;                                     // Register number 5 Z_MSB
        Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MAG3110_ADDRESS,Data,2);        // Read register contents
        Z = (Data[0]<<8) +Data[1];                        // 2 bytes returned, shift by 8 bits and AND Least signficant byte to get 16 bit number
        
        //Update the X,Y and Z characteristics after they have been read
        updateMagX(X);
        updateMagY(Y);
        updateMagZ(Z);        
        
    }
//Private variables of class 
private:
    BLEDevice &ble;
    ReadOnlyGattCharacteristic<int16_t>  MagX;
    ReadOnlyGattCharacteristic<int16_t>  MagY;
    ReadOnlyGattCharacteristic<int16_t>  MagZ;
};

#endif /* #ifndef __BLE_ACCEL_SERVICE_H__ */
