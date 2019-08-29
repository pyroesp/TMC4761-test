#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tmc/ic/TMC4671/TMC4671.h"

uint8_t spi_transfer(uint8_t data, uint8_t lastTransfer){
    // Make your own spi transfer function
    return lastTransfer;
}

uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer){
    // defined as external function in TMC4671.c
    // see also the evaluation board demo files
    if (motor){
        return spi_transfer(data, lastTransfer);
    }else{
        return 0;
    }
}


int main()
{
    // test function
    tmc4671_setTargetTorque_raw(1, 0xAABB);
    return 0;
}
