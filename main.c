
/** Proof of Concept **/
/**    Not Tested    **/

#include "tmc/ic/TMC4671/TMC4671.h"

/**
The TMC4671_eval code uses DEFAULT_MOTOR and TMC4671_MOTOR defines, why I don't know...
They ignore TMC4671_MOTOR and only use DEFAULT_MOTOR

Also, the TMC4671_MOTORS define can be found in the TMC4671_Constants.h
This define tells the programmer how many motors you can drive from the chip
For example: a TMC2041 has a TMC2041_MOTORS define set to 2, because it can drive 2 motors

The TMC4671 only has 1 motor, so the motor variable can be ignored in the library
unless the circuit uses more than one TMC4671. In that case use the motor variable to select
the appropriate SS signal in the readwriteByte function as follows:

    if (motor == 0)
        digitalWrite(SS0, LOW);
    else if (motor == 1)
        digitalWrite(SS1, LOW);

If you use multiple different Trinamic chips where some chips can drive more than one motor, then you're SOL
**/


// define SS pin for the trinamic chip
#define SS IO_pin
// setup the spi settings
SPISettings settingsA(something, something, something);

// Make your own spi transfer function
uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer){
    /**
        ignore motor variable, unless you have multiple TMC4671 chips,
        then use it to clear/set the SS for each motor (I guess ??)
    **/

    uint8_t received_data = 0;

    SPI.settings(settingsA); // apply SPI settings
    digitalWrite(SS, LOW); // set SS low to enable data transfer to chip
    received_data = SPI.transfer(data); // send data byte
    // Assuming SPI.transfer sends a byte and waits until it's done before going to the next line
    // if not, then add loop until byte is sent

    // last transfer is a boolean that tells the readwriteByte when the last byte is being transferred,
    // which means that if the boolean is set, you can disable the transfer (with SS)
    if (lastTransfer) // if last byte in transfer packet
        digitalWrite(SS,HIGH); // set SS high to disable data transfer

    return received_data;
}

// arduino setup
setup(){
    // Do SPI setup here
    SPI.begin();

    // Do motor setup here
    tmc4671_setMotorType(0, motorType);
    tmc4671_setPolePairs(0, polePairs);
}

// arduino loop
loop()
{
    // do loop stuff here
}
