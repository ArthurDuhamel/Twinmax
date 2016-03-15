#include "can.h"
#include "bluetooth.h"
#include "inputs.h"
#include "average.h"
#include "engine.h"

void adc_init() {

    // Set analog inputs for sensors (& battery ?)
    ANSA = ANSA || 0b0000000000010011;
    ANSB = ANSB || 0b1111000000000000;
    ANSC = ANSC || 0b0000000000000011; // Modifié (ancien : 0b 0000 0000 0000 0001 )

    AD1CON1bits.ADON = 0; //Disable CAN for configuration
    AD1CON1bits.ADSIDL = 1; //Stop when in idle mode
    AD1CON1bits.MODE12 = 1; // 12 BITS CAN
    AD1CON1bits.FORM = 0b00; //Absolute decimal, unsigned, right-justified
    AD1CON1bits.SSRC = 0b0111; //Auto-Conversion mode
    AD1CON1bits.ASAM = 0; //Wait for timer to start
    AD1CON1bits.SAMP = 0; //Wait for timer and ASSAM
    AD1CON1bits.DONE = 0; // Clear CAN flag

    AD1CON2bits.PVCFG = 0b00; //Set AVDD as reference
    AD1CON2bits.NVCFG0 = 0; //Set AVSS as reference
    AD1CON2bits.OFFCAL = 0; //Connect to normal inputs
    AD1CON2bits.BUFREGEN = 1; // No FIFO buffering
    AD1CON2bits.SMPI = 0b01000; // Interruption occurs after 9 conversions
    AD1CON2bits.BUFM = 0; // FIFO buffer successive fill mode
    AD1CON2bits.ALTS = 0; // Always use sample A channel
    AD1CON2bits.CSCNA = 1; // Scan inputs

    AD1CON3bits.ADRC = 0; //Use internal clock
    AD1CON3bits.SAMC = 0b00010; // Set as 2 * T_AD
    AD1CON3bits.ADCS = 0b00001001; // Set as 10 T_AD

    AD1CON5bits.ASEN = 0; //Auto-scan disable
    AD1CON5bits.LPEN = 0; // Full power enabled after scan
    AD1CON5bits.CTMREQ = 0; // CTMU disable
    AD1CON5bits.BGREQ = 0; //Band gap is not enable
    //AD1CON5bits.VRSREQ = 0; // On-chip regulator is not enable by the ADC
    AD1CON5bits.ASINT = 0b00; // No interrupt 
    AD1CON5bits.WM = 0b00; //Classic buffering
    AD1CON5bits.CM = 0b00; //Less than mode

    AD1CHS = 0b0001111000011110; //Set Vss and AVdd as references

    // Select input pins
    /*
     * RB12 = capteur 1          =  AN12
     * RB13 = capteur 1 moyenne  =  AN11
     * RB14 = capteur 2          =  AN10
     * RB15 = capteur 2 moyenne  =  AN9
     * RA0  = capteur 3          =  AN0
     * RA1  = capteur 3 moyenne  =  AN1
     * RC0  = capteur 4          =  AN6
     * RA4  = capteur 4 moyenne  =  AN16
     * RC1(PIN26) = Batterie     =  AN7
     */

    AD1CSSL = 0b0001111011000011;
    AD1CSSH = 0b0000000000000001;

    IFS0bits.AD1IF = 0; // Reset interrupt flag
    IEC0bits.AD1IE = 1; // Enable interrupt    
    IPC3bits.AD1IP = 0b100; // Interrupt priority set to 4    
    AD1CON1bits.ADON = 1; // Enable CAN
}

void adc_launch() {
    // Launch the 8 (9 w/ battery) conversions
    AD1CON1bits.ASAM = 1;
}

void __attribute__((__interrupt__, __auto_psv__)) _ADC1Interrupt(void) {
    // extern struct movingAverage * average1;
    // extern struct movingAverage * average2;
    extern int isConnected;
    extern int canSend;
    extern unsigned short batteryLevel;

    AD1CON1bits.ASAM = 0;
    IFS0bits.AD1IF = 0; //reset interrupt flag

    // average_add_value(average1, SENSOR4BUF);
    // average_add_value(average2, SENSOR3BUF);

    //average_add_values(SENSOR1AVGBUF, SENSOR2AVGBUF, SENSOR3AVGBUF, SENSOR4AVGBUF);  
    average_add_values(SENSOR4AVGBUF, SENSOR3AVGBUF, SENSOR2AVGBUF, SENSOR1AVGBUF);

    batteryLevel = (BATTERYLEVEL + batteryLevel) / 2;

    if (isConnected == 0xFF) {
        isConnected = 0x00;
        if (canSend == 0) {
            __delay_ms(3000);
            canSend = 1;
        } else {
            // __delay_ms(1000);
            canSend = 0;
        }
    }
    if (canSend == 1) {
        ble_start();
    }
}