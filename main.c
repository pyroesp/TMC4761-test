#include "tmc/ic/TMC4671/TMC4671.h"

SPISettings settingsA(something, something, something);

setup(){
    // Do SPI setup here
    SPI.begin();

    // Do motor setup here
    tmc4671_setMotorType(motor, motorType);
    tmc4671_setPolePairs(motor, polePairs);
}

uint8_t spi_transfer(uint8_t data, uint8_t lastTransfer){
    // Make your own spi transfer function
    uint8_t received_data = 0;

    SPI.settings(settingsA); // apply SPI settings
    digitalWrite(SS, LOW); // set SS low to enable data transfer to chip
    received_data = SPI.transfer(data); // send data byte
    // Assuming SPI.transfer send a byte and waits until it's done
    if (lastTransfer) // if last byte in transfer packet
        digitalWrite(SS,HIGH); // set SS high to disable data transfer

    return received_data;
}

uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer){
    // defined as external function in TMC4671.c
    // see also the evaluation board demo files IIRC
    if (motor == DEFAULT_MOTOR){
        return spi_transfer(data, lastTransfer);
    }else{
        return 0;
    }
}

loop()
{
    // do loop stuff here
}
