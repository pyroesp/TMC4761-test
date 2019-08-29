#include "tmc/ic/TMC4671/TMC4671.h"

uint8_t spi_transfer(uint8_t data, uint8_t lastTransfer){
    // Make your own spi transfer function

    return lastTransfer;
}

uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer){
    // defined as external function in TMC4671.c
    // see also the evaluation board demo files IIRC
    if (motor){
        return spi_transfer(data, lastTransfer);
    }else{
        return 0;
    }
}

setup(){
    // Do SPI setup here

    // Do motor setup here
    tmc4671_setMotorType(motor, motorType);
    tmc4671_setPolePairs(motor, polePairs);
}

loop()
{
    // do loop stuff here
}
