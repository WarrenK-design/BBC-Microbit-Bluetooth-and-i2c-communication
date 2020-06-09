// Accelerometer: MMA8653 
// Datasheet:     https://www.nxp.com/docs/en/data-sheet/MMA8653FC.pdf 
#ifndef __BLE_ACCEL_SERVICE_H__
#define __BLE_ACCEL_SERVICE_H__
#include <mbed.h>


// This enables the i2c bus using mbeds i2c api 
// The construtor takes in the pin locations for the SDA and the SCL
// Serial Data(SDA)  - Allows the master and slave to send and recieve data
// Serial Clock(SCL) - i2c is synchronous and a clock signal must be shared between master and slave   
// SDA - P0_30
// SCL - P0_0
I2C i2c(P0_30, P0_0); 

// Enable Universal Asynchronous Receiver/Transmitter (UART)
// UART enables the bbc to communicate with the PC  
// Two channels are set up to transmit(USBTX) and recive data(USBRX) 
Serial pc(USBTX,USBRX);

// The standard i2c slave address for MMA8653FC is 0x1D or 0011101 - reference section 5.8, page 18 of data sheet 
// The MMA8653_ID refers to the value of the WHOAMI byte in the register 0x0D, it has a hex value of 0x5a
const int MMA8653_ADDRESS = (0x1d<<1); 
const int MMA8653_ID = 0x5a;

// Assigning the pins of the LEDS as outputs  
// LED's in BBC microbit laid out in 9 columns and 3 rows 
// The columns are intialsed to have a starting value of 1 
// The rows have a starting value of 0 
// Rows are anodes of the LED 
// Columns are cathodes of LED 
// To light LED set the column to 0 and row to 1 
DigitalOut col1(P0_4,1);
DigitalOut col2(P0_5,1);
DigitalOut col3(P0_6,1);
DigitalOut col4(P0_7,1);
DigitalOut col5(P0_8,1);
DigitalOut col6(P0_9,1);
DigitalOut col7(P0_10,1);
DigitalOut col8(P0_11,1);
DigitalOut col9(P0_12,1);

DigitalOut row1(P0_13);
DigitalOut row2(P0_14);
DigitalOut row3(P0_15);


///ACCELService///
// Contains all the functions and class variables associated with Acellerometer
// Creates the Acellerometer service in the BLE profile 
// Reads the X,Y and Z characteristics for the Acelllerometer service 
class ACCELService {
public:
    //Universal Unique Identification numbers for Accelerometer//
    //The accelerometer service has a UUID of 0xA012
    //Each of the characteristics have UUID's
    //UUID X plane Characteristic - 0xA013
    //UUID Y plane Characteristic - 0xA014
    //UUID Z plane Characteristic - 0xA015
    const static uint16_t ACCEL_SERVICE_UUID = 0xA012;
    const static uint16_t ACCEL_X_CHARACTERISTIC_UUID = 0xA013;
    const static uint16_t ACCEL_Y_CHARACTERISTIC_UUID = 0xA014;
    const static uint16_t ACCEL_Z_CHARACTERISTIC_UUID = 0xA015;
    
    //ACCELService Constructor//
    // Will create the Accelerometer service for bluetooth profile 
    // Assigns the UUID's decalred for Accelerometer and Characteristics 
    // Wakes up the Accelerometer by writing to control register 1 a value of 1
    ACCELService(BLEDevice &_ble, int16_t initialValueForACCELCharacteristic) :
        ble(_ble), AccelX(ACCEL_X_CHARACTERISTIC_UUID, &initialValueForACCELCharacteristic),AccelY(ACCEL_Y_CHARACTERISTIC_UUID, &initialValueForACCELCharacteristic),AccelZ(ACCEL_Z_CHARACTERISTIC_UUID, &initialValueForACCELCharacteristic)
    {
        // Assign the gatt characteristics to a GattCharacteristic instance 
        GattCharacteristic *charTable[] = {&AccelX,&AccelY,&AccelZ};
        // Create an instance of a service for the Accelerometer and associate the characteristics with it
        GattService         AccelService(ACCEL_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        // Add the service to the ble profile 
        ble.addService(AccelService);
        
        // Wake the accelerometer from sleep mode by writing 1 to register number 0x2a    
        char Data[8]; // Declare a buffer for data transfer    
        int Status;
        Data[0]=0x2a; // Control regester 1 address 
        Data[1]=1;
        Status = i2c.write(MMA8653_ADDRESS,Data,2);  // Write data to register    
    }

    GattAttribute::Handle_t getValueHandle() const {
        return AccelX.getValueHandle();
    }
    
    // Updates the value of the X characteristic 
    void updateAccelX(uint16_t newValue) {
        ble.gattServer().write(AccelX.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }
    // Updates the value of the Y characteristic 
    void updateAccelY(uint16_t newValue) {
        ble.gattServer().write(AccelY.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }
    // Updates the value of the Z characteristic 
    void updateAccelZ(uint16_t newValue) {
        ble.gattServer().write(AccelZ.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }
    
    
    
/// MMA8653_ReadAccelX ///
// Will read the value value of accleration in the X plane from the MMA8653
// The current value of accleration in the x plane is returend from function 
int16_t MMA8653_ReadAccelX()
{
    //Data   - Bufer for data transfer 
    //Status - Return status of read/write i2c operations
    //X      - The value of accleration in X plane 
    char Data[8]; // Declare a buffer for data transfer    
    int Status;
    int16_t X;
    
    //Data[0] is assigned a value of 0x01 as this will be the 8 MSB's of the accleration in the x plane 
    //There is a 10 bit A/D converter, the 2 remaining LSB's are in register 0x02
    Data[0]=0x01; 
    
    //First write to the register number 0x01 to the MMA865
    Status = i2c.write(MMA8653_ADDRESS,Data,1,true);
    //Now read the value of the register 0x01 from the MMA865  
    Status = i2c.read(MMA8653_ADDRESS,Data,2);
    
    //The value of X will be a 10 bit number but only the 8 MSB's have been read 
    //If the remining 2 LSB's could be read from register 0x02  
    X = Data[0];            
    X = (X << 8) + Data[1]; //16 bit number 
    X = X >> 6;             //10 bit number 
    return X;

}

/// MMA8653_ReadAccelY ///
// Will read the value value of accleration in the Y plane from the MMA8653
// The current value of accleration in the Y plane is returend from function 
int16_t MMA8653_ReadAccelY()
{
    //Data   - Bufer for data transfer 
    //Status - Return status of read/write i2c operations
    //Y      - The value of accleration in X plane 
    char Data[8];   
    int Status;
    int16_t Y;
    
    //Data[0] is assigned a value of 0x03 as this will be the 8 MSB's of the accleration in the y plane 
    //There is a 10 bit A/D converter, the 2 remaining LSB's are in register 0x04
    Data[0]=0x03;
    
    //First write to the register number 0x03 to the MMA865
    Status = i2c.write(MMA8653_ADDRESS,Data,1,true);
    //Now read the value of the register 0x03 from the MMA865  
    Status = i2c.read(MMA8653_ADDRESS,Data,2);
    
    //The value of Y will be a 10 bit number but only the 8 MSB's have been read 
    //If the remining 2 LSB's could be read from register 0x04
    Y = Data[0];
    Y = (Y << 8) + Data[1]; //16 bit number 
    Y = Y >> 6;             //10 bit number 
    return Y;
}

/// MMA8653_ReadAccelZ  ///
// Will read the value value of accleration in the Z plane from the MMA8653
// The current value of accleration in the Z plane is returend from function 
int16_t MMA8653_ReadAccelZ()
{
    //Data   - Bufer for data transfer 
    //Status - Return status of read/write i2c operations
    //Z      - The value of accleration in Z plane 
    char Data[8];   
    int Status;
    int16_t Z;
    
    //Data[0] is assigned a value of 0x05 as this will be the 8 MSB's of the accleration in the Z plane 
    //There is a 10 bit A/D converter, the 2 remaining LSB's are in register 0x06
    Data[0]=0x05;
    
    //First write to the register number 0x05 to the MMA865
    Status = i2c.write(MMA8653_ADDRESS,Data,1,true);
    //Now read the value of the register 0x05 from the MMA865 
    Status = i2c.read(MMA8653_ADDRESS,Data,2);
    
    //The value of Z will be a 10 bit number but only the 8 MSB's have been read 
    //If the remining 2 LSB's could be read from register 0x06
    Z = Data[0];
    Z = (Z << 8) + Data[1]; //16 bit number
    Z = Z >> 6;             //10 bit number 
    return Z;
}
    
    ///poll///
    //Poll will get the value of the acellerometer for the x, y and z planes
    //by calling the functions MMA8653_ReadAccelX(), MMA8653_ReadAccelY and MMA8653_ReadAccelZ()
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
        // X - x plane value 
        // Y - y plane value 
        // Z - z plane value
        int16_t X;
        int16_t Y;
        int16_t Z;
        
        //Get each of the values 
        Z=MMA8653_ReadAccelZ();
        X=MMA8653_ReadAccelX();
        Y=MMA8653_ReadAccelY();
        
        // Update the characteristcs for each 
        // of these values in bluetooth profile 
        updateAccelX(X);
        updateAccelY(Y);
        updateAccelZ(Z);  
        
        //Will write values to COM port useful for debug     
        pc.printf("X=%d Y=%d Z=%d \n\r",X,Y,Z); 
    }
    
    ///Direction///
    // This function will read the values of the X and Y palne of the acelerometer
    // Based upon these values it can be determined what direction the bbc microbit is tilted 
    // Functions are then called to display arrows on the LED display in the tilted direction  
    void Direction(){ 
        int16_t X;
        int16_t Y;      
        X=MMA8653_ReadAccelX();
        Y=MMA8653_ReadAccelY();
         
    //-X and -Y
    //Tilt is to the bottom right    
    if ((X<=0)&&(Y<=0)){
        //X is less than Y so tilt to right 
        if((X)<(Y)){
            Right();
            }
        //Y is less than X so tilt to down     
        else{
            Down();
            }    
            }
            
    //-X and +Y tilt to top left         
    if ((X<=0)&&(Y>=0)){
        //X is greater than Y so tilt to right
        if(abs(X)>(Y)){
            Right();
            }
        //Y is greater than X so tilt to Up
        else{
            Up();
            }    
            }
    //+X and +Y tilt to top right        
    if ((X>=0)&&(Y>=0)){
        //X is greater than Y so tilt to left
        if((X)>(Y)){
            Left();
            }
        //Y is greater than X so tilt to Up
        else{
            Up();
            }    
            }
    //+X and -Y tilt to bottom left        
    if ((X>=0)&&(Y<=0)){ 
        //X is greater than Y so tilt to left
        if((X)>(abs(Y))){
            Left();
            }
        //Y is greater than X so tilt to Down
        else{
            Down();
            }    
            }
 }       
 
    // Up //
    // Will display a Up arrow on LED display    
    // Rows are anodes of the LED 
    // Columns are cathodes of LED 
    // Should not have more than 1 row powered at once  
    void Up(){
             //ROW1//     
             row1=1;  
             col2=0;
             col6=0;
             wait(0.01);
             row1=0;
             col2=1;
             col6=1;
             
             //ROW2//
             row2=1;
             col3=0;
             col2=0;
             col1=0;
             wait(0.01);
             row2=0;   
             col3=1;
             col2=1;
             col1=1;
             
             //ROW3//
             row3=1;
             col5=0;
             col7=0;
             col6=0;
             col1=0;
             wait(0.01);
             row3=0;
             col5=1;
             col7=1;
             col6=1;
             col1=1; 
         }
    //Left//
    //Will display a Left arrow on LED display        
    // Rows are anodes of the LED 
    // Columns are cathodes of LED 
    // Should not have more than 1 row powered at once  
    void Left(){
            //ROW1//
            row1=1;
            col2=0;
            col7=0;
            col9=0;
            wait(0.01);
            row1=0;
            col2=1;
            col7=1;
            col9=1;
            
            //ROW2//
            row2=1;
            col1=0;
            col2=0;
            col3=0;
            wait(0.01);
            row2=0;
            col1=1;
            col2=1;
            col3=1;
            
            //ROW3//
            row3=1;
            col1=0;
            col5=0;
            col9=0;
            wait(0.01);
            row3=0;
            col1=1;
            col5=1;
            col9=1;
            }
        
    //Right//
    //Will display a Right arrow on LED display    
    // Rows are anodes of the LED 
    // Columns are cathodes of LED 
    // Should not have more than 1 row powered at once  
    void Right(){
        //ROW1//
        row1=1;
        col2=0;   
        col5=0;
        col9=0;  
        wait(0.01);
        row1=0;
        col2=1;   
        col5=1;
        col9=1;
        
        //ROW2//
        row2=1;
        col1=0;
        col2=0;
        col3=0;
        wait(0.01);
        row2=0;
        col1=1;
        col2=1;
        col3=1;
        
        //ROW3//
        row3=1;
        col1=0;
        col7=0;
        col9=0; 
        wait(0.01);
        row3=0;
        col1=1;
        col7=1;
        col9=1; 
               
               }
               
    //Down//
    //Will display a Right arrow on LED display    
    // Rows are anodes of the LED 
    // Columns are cathodes of LED 
    // Should not have more than 1 row powered at once   
    void Down(){
        //ROW1//
        row1=1;
        col2=0;
        col5=0;
        col6=0;
        col7=0;
        wait(0.01);
        row1=0;
        col2=1;
        col5=1;
        col6=1;
        col7=1;
        
        //ROW2//
        row2=1;
        col1=0;
        col2=0;
        col3=0;
        wait(0.01);
        row2=0;
        col1=1;
        col2=1;
        col3=1;
        
        //ROW3//
        row3=1;
        col1=0;
        col6=0;
        wait(0.01);
        row3=0;
        col1=1;
        col6=1;
    }
    
//Private variables of the class 
private:
    BLEDevice &ble;
    ReadOnlyGattCharacteristic<int16_t>  AccelX;
    ReadOnlyGattCharacteristic<int16_t>  AccelY;
    ReadOnlyGattCharacteristic<int16_t>  AccelZ;
};

#endif /* #ifndef __BLE_ACCEL_SERVICE_H__ */
