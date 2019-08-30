
/** Proof of Concept **/
/**    Not Tested    **/

#include <stdint.h> // includes uint32_t & co

#include "tmc/ic/TMC4671/TMC4671.h"

#define TMC4671_ID 0x34363731

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
#define SS <define SS pin here>
// setup the spi settings
SPISettings settingsTMC4671(1000000, MSBFIRST, SPI0_MODE3);

// Make your own spi transfer function, used by the TMC-API
uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer){
    /**
        ignore motor variable, unless you have multiple TMC4671 chips,
        then use it to clear/set the SS for each motor (I guess ??)
    **/

    uint8_t received_data = 0;
    SPI.beginTransaction(settingsTMC4671);
    digitalWrite(SS, LOW); // set SS low to enable data transfer to chip
    received_data = SPI.transfer(data); // send data byte
    // Assuming SPI.transfer sends a byte and waits until it's done before going to the next line
    // if not, then add loop until byte is sent

    // last transfer is a boolean that tells the readwriteByte when the last byte is being transferred,
    // which means that if the boolean is set, you can disable the transfer with SS
    if (lastTransfer) // if last byte in transfer packet
        digitalWrite(SS,HIGH); // set SS high to disable data transfer

    return received_data;
}

// arduino setup
setup(){
    uint32_t tmc_id; // ID return variable from 4671
    uint32_t tmc_input_raw; // raw input from tmc4671
    
    int serial_read = 0; // used for reading serial communication from PC to MCU
    uint8_t exit_input_test = 0; // used for input test loop
    
    Serial.begin(115200); // enable serial at 115200 baud
    
    /** See chapter 11 for setup guideline **/
    
    /* Step 0: Setup SPI */
    Serial.println("Step 0:");
    SPI.begin();
    // simple SPI datagram, see p15
    tmc4671_writeInt(0, TMC4671_CHIPINFO_ADDR, CHIPINFO_ADDR_SI_TYPE); // (motor, addr -> register.h, data -> variants.h)
    tmc_id = tmc4671_readInt(0, TMC4671_CHIPINFO_DATA); // (motor, addr)
    
    if (tmc_id != TMC4671_ID){ // if received ID not correct
        // print some debug stuff and loop
        Serial.print("Trinamic chip not detected, read ID: ");
        Serial.println(tmc_id, HEX);
        Serial.println("Check connection?\nCheck endianness?\nLooping now...");
        for(;;);
    }
    
    Serial.println("Step 0 done.");
    /* Step 1: Check input connections */
    Serial.println("Step 1:");
    /*
        loop while testing HALL switches, ENABLE, STEP and DIR inputs
          > use 'e' to exit loop and continue to step 2
          > use 'r' to read the inputs
    */
    serial_read = 0;
    exit_input_test = 0;
    while(!exit_input_test){ // loop while testing inputs
        if (Serial.available() > 0) // read serial input if data available
            serial_read = Serial.read();
        if (serial_read = 'e'){ // use 'e' to exit 
            exit_input_test = 1;
        } else if (serial_read == 'r') { // use 'r' to read the inputs 
            tmc_input_raw = tmc4671_readInt(0, TMC4671_INPUT_RAW); // read raw input
            Serial.print("HALL_UX = ");
            // mask off all bits but HALL_UX from returned raw input
            // shift the value left to get a 0 or 1 value
            Serial.println(tmc_input_raw & TMC4671_HALL_UX_OF_HALL_RAW_MASK >> TMC4671_HALL_UX_OF_HALL_RAW_SHIFT);
            // mask off all bits but HALL_V from returned raw input
            // shift the value left to get a 0 or 1 value
            Serial.print("HALL_V = ");
            Serial.println(tmc_input_raw & TMC4671_HALL_V_OF_HALL_RAW_MASK >> TMC4671_HALL_V_OF_HALL_RAW_SHIFT); 
            // mask off all bits but HALL_WY from returned raw input
            // shift the value left to get a 0 or 1 value
            Serial.print("HALL_WY = ");
            Serial.println(tmc_input_raw & TMC4671_HALL_WY_OF_HALL_RAW_MASK >> TMC4671_HALL_WY_OF_HALL_RAW_SHIFT); 
            // mask off all bits but ENABLE from returned raw input
            // shift the value left to get a 0 or 1 value
            Serial.print("ENABLE = ");
            Serial.println(tmc_input_raw & TMC4671_ENABLE_IN_RAW_MASK >> TMC4671_ENABLE_IN_RAW_SHIFT); 
            // mask off all bits but STEP from returned raw input
            // shift the value left to get a 0 or 1 value
            Serial.print("STEP = ");
            Serial.println(tmc_input_raw & TMC4671_STP_OF_DIRSTP_RAW_MASK >> TMC4671_STP_OF_DIRSTP_RAW_SHIFT); 
            // mask off all bits but DIRECTION from returned raw input
            // shift the value left to get a 0 or 1 value
            Serial.print("DIR = ");
            Serial.println(tmc_input_raw & TMC4671_DIR_OF_DIRSTP_RAW_MASK >> TMC4671_DIR_OF_DIRSTP_RAW_SHIFT); 
        }
    }
    
    Serial.println("Step 1 done.");
    /* Step 2: Setup of PWM and Gatedriver configuration */
    Serial.println("Step 2:");
    
    Serial.println("Step 2 done.");
    /* Step 3: Open Loop Mode */
    Serial.println("Step 3:");
    
    Serial.println("Step 3 done.");
    /* Step 4: Setup of ADC for current measurement */
    Serial.println("Step 4:");
    
    Serial.println("Step 4 done.");
    /* Step 5: Setup of Feedback Systems */
    Serial.println("Step 5:");
    
    Serial.println("Step 5 done.");
    /* Step 6: Setup of FOC Controllers */
    Serial.println("Step 6:");
    
    Serial.println("Step 6 done.");
    /* Step 7: Advanced Functions */
    Serial.println("Step 7:");
}

// arduino loop
loop()
{
    // do loop stuff here
}
