
/** Proof of Concept **/
/**    Not Tested    **/

#include <stdint.h> // includes uint32_t & co
#include <string.h> // strstr
#include <stdio.h> // sscanf

#include "tmc/ic/TMC4671/TMC4671.h"

#define TMC4671_ID 0x34363731 // "4671"
#define TMC4671_BOB_FREQ 25000000 // 25MHz


#define SERVO_PWM_FREQ 100000 // 100kHz; recommended between 25~100kHz
#define SERVO_POLE_PAIRS 7

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
    
    SPI.endTransaction();

    return received_data;
}

// arduino setup
setup(){    
    Serial.begin(115200); // enable serial at 115200 baud
    
    /** See chapter 11 for setup guideline **/
    
    /* Step 0: Setup SPI */
    Serial.println("Step 0:");
    SPI.begin();
    // simple SPI datagram, see p15
    tmc4671_writeInt(0, TMC4671_CHIPINFO_ADDR, CHIPINFO_ADDR_SI_TYPE); // (motor, addr -> register.h, data -> variants.h)
    uint32_t tmc_id = tmc4671_readInt(0, TMC4671_CHIPINFO_DATA); // (motor, addr)
    
    if (tmc_id != TMC4671_ID){ // if received ID not correct
        // print some debug stuff and loop
        Serial.print("Trinamic chip not detected, read ID was : 0x");
        Serial.println(tmc_id, HEX);
        Serial.println("Should be 0x34363731\nCheck connection?\nCheck endianness?\nLooping now...");
        for(;;);
    }
    
    Serial.println("Step 0 done.");
    /* ------------------------------------------------------------------------------------- */
    /* Step 1: Check input connections */
    Serial.println("Step 1:");
    /*
        loop while testing HALL switches, ENABLE, STEP and DIR inputs
          > send 'e' to exit loop and continue to step 2
          > send 'r' to read the inputs once
    */

    uint8_t serial_read; // used for reading serial communication from PC to MCU
    uint8_t exit_input_test = 0; // used for input test loop
    uint32_t tmc_input_raw = 0; // raw input from tmc4671
    while(!exit_input_test){ // loop while testing inputs
        serial_read = 0;
        if (Serial.available() > 0) // read serial input if data available
            serial_read = (uint8_t)Serial.read();
        if (serial_read = 'e'){ // send 'e' to exit 
            exit_input_test = 1;
        } else if (serial_read == 'r') { // send 'r' to read the inputs 
            tmc_input_raw = tmc4671_readInt(0, TMC4671_INPUT_RAW); // read raw input
            Serial.print("HALL_UX = ");
            // mask off all bits but HALL_UX from returned raw input
            // shift the value left to get a 0 or 1 value
            // MASK and SHIFT defines can be found in fields.h
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
    /* ------------------------------------------------------------------------------------- */
    /* Step 2: Setup of PWM and Gatedriver configuration */
    Serial.println("Step 2:");
    tmc4671_setMotorType(0, TMC4671_THREE_PHASE_BLDC); // (motor, motorType -> define in registers.h)
    tmc4671_setPolePairs(0, SERVO_POLE_PAIRS); // pole pair defined by user
    
    // Setup PWM polarity here
    // uint32_t pwm_polarities = tmc4671_readInt(0, TMC4671_PWM_POLARITIES); // read default values
    // setBit(pwm_polarities, TMC4671_PWM_POLARITIES_7_SHIFT); // overcurrent signal polarity (default:0)
    // tmc4671_writeInt(0, TMC4671_PWM_POLARITIES, pwm_polarities); // write new polarity value to register
    
    // Setup PWM_MAXCNT which controls the PWM freq; recommended between 25kHz and 100kHz
    // fPWM is calculated as follows : (4 * fCLK)/(PWM_MAXCNT - 1)
    uint32_t max_cnt= 1 + (4 * TMC4671_BOB_FREQ)/SERVO_PWM_FREQ;
    tmc4671_writeInt(0, TMC4671_PWM_MAXCNT, max_cnt);
    
    // Setup Break-Before-Make -> delay between HS/LS MOSFET switching to prevent power supply short circuit
    // NOTE: MOSFET driver might have built-in BBM
    // NOTE: max BBM value is 255 -> 2.55us
    // uint8_t high_side_bbm = 20; // default
    // uint8_t low_side_bbm = 20; // default
    // tmc4671_writeInt(0, TMC4671_PWM_BBM_H_BBM_L, high_side_bbm << TMC4671_PWM_BBM_H_SHIFT | low_side_bbm);
    
    // Turn ON PWM in centered mode for FOC (= 7)
    tmc4671_writeInt(0, TMC4671_PWM_SV_CHOP, 7);
    
    Serial.println("Step 2 done.");
    /* ------------------------------------------------------------------------------------- */
    /* Step 3: Open Loop Mode */
    Serial.println("Step 3:");
    // In the Open Loop Mode the motor is turned by applying voltage to the motor, useful for test and setup of ADC and position sensor
    // POSITION_SELECTION and VELOCITY_SELECTION are default 0, which choses PHI_E based on the PHI_E_SELECTION register
    tmc4671_writeInt(0, TMC4671_PHI_E_SELECTION, TMC4671_PHI_E_OPEN_LOOP); // set PHI_E_SELECTION in open loop
    
    tmc4671_switchToMotionMode(0, TMC4671_MOTION_MODE_UQ_UD_EXT); // set motion mode to UQ_UD_EXT
    // With UD_EXT the applied voltage can be regulated upwards until the motor starts to turn
    int16_t uq = 0;
    int16_t ud = 0;
    uint8_t exit_ud_loop = 0;
    char str[16];
    int idx = 0;
    Serial.println("Entering UD loop, send exit to go to next step.");
    Serial.println("Enter UD value until motor starts to turn (-32658/32657): ");
    while(!exit_ud_loop){
        while(Serial.available() == 0); // wait for UART transmission from PC to MCU
        idx = 0;
        while(Serial.available() > 0){ // read transmission
            str[idx] = (char)Serial.read();
            idx++
            if (idx >= 16){ // if more than 16 chars read, then flush and try again
                Serial.flush();
                break;
            }
        }
        if (idx < 16){ // check only if valid data received
            if (strcmp(str, "exit")){
                exit_ud_loop = 1;
            }else{ // if string is not "exit", convert to integer and set register
                sscanf(str, "%d", &ud);
                tmc4671_writeInt(0, TMC4671_UQ_UD_EXT, uq << TMC4671_UQ_EXT_SHIFT | ud);
                Serial.println("Enter UD value until motor starts to turn (-32658/32657): "); // prompt user for new UD value again
            }
        }
    }
    
    // Acceleration and Velocity need to be tested the same way as UD ??
    
    Serial.println("Step 3 done.");
    /* ------------------------------------------------------------------------------------- */
    /* Step 4: Setup of ADC for current measurement */
    Serial.println("Step 4:");
    
    Serial.println("Step 4 done.");
    /* ------------------------------------------------------------------------------------- */
    /* Step 5: Setup of Feedback Systems */
    Serial.println("Step 5:");
    
    Serial.println("Step 5 done.");
    /* ------------------------------------------------------------------------------------- */
    /* Step 6: Setup of FOC Controllers */
    Serial.println("Step 6:");
    
    Serial.println("Step 6 done.");
    /* ------------------------------------------------------------------------------------- */
    /* Step 7: Advanced Functions */
    Serial.println("Step 7:");
    
    Serial.println("Step 7 done.");
    /* ------------------------------------------------------------------------------------- */
}

// arduino loop
loop()
{
    // do loop stuff here
}
