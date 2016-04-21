#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "bluetooth.h"
#include "inputs.h"


#define FCY 16000000UL
#include "libpic30.h"

volatile UINT8_T value[9];
volatile int packet_number;

UINT8_T shiftLeft(unsigned short data) {
    UINT8_T bus = ((data & 0b111111000000) >> 6) << 1;
    return (bus);
}

UINT8_T shiftRight(unsigned short data) {
    UINT8_T bus = (data & 0b000000111111) << 1;
    return (bus);
}

void ble_send() {

    if (packet_number == 0) {
        U1TXREG = value[0];
        U1TXREG = value[1];
        U1TXREG = value[2];
        U1TXREG = value[3];
    } else {
        if (packet_number == 1) {
            U1TXREG = value[4];
            U1TXREG = value[5];
            U1TXREG = value[6];
            U1TXREG = value[7];
        } else {
            if (packet_number == 2) {
                U1TXREG = value[8];
            } else {
                return;
            }
        }
    }
    packet_number++;
}

void ble_frame_formating() {

    value[0] = 0b10000000; //entête
    value[1] = shiftLeft(SENSOR1BUF);
    value[2] = shiftRight(SENSOR1BUF);
    value[3] = shiftLeft(SENSOR2BUF);
    value[4] = shiftRight(SENSOR2BUF);
    value[5] = shiftLeft(SENSOR3BUF);
    value[6] = shiftRight(SENSOR3BUF);
    value[7] = shiftLeft(SENSOR4BUF);
    value[8] = shiftRight(SENSOR4BUF);
}

void ble_start() {
    packet_number = 0;
    ble_frame_formating();
    ble_send();
    return;
}

void ble_config() {
    initBTModule();

    
    ////uart setup///////
    U1BRG = 0x0010;
    U1MODE = 0b1000100000001000; //OK 
    U1STA = 0b0010010100010000; //bit 15-13 = 01 : flag passe à 1 lorsque que le TSR est vide // Receviver not active
    //U1STA = 0b0010010100000000; //Receive active
    ////uart intertupt/////
    IPC3bits.U1TXIP = 0b011; // priority = 3
    IFS0bits.U1TXIF = 0; //interrupt has request
    IEC0bits.U1TXIE = 1; // interrupt request are enabled 
    
    //Receiver
    IPC2bits.U1RXIP = 0b011;
    IFS0bits.U1RXIF = 0;//clear flag !
    IEC0bits.U1RXIE = 1;
    
    ANSA = 0b0000000000000000;
    ANSB = 0b0000000000000000;
    //////////////////////
}

void UARTInit230400(void) {
    CLKDIV = 0x0000;
    //    TRISB = 0x0000;     // port B en sortie

    U1BRG = 0x0010; //pour mode fast


    U1MODE = 0b1000100000001000; //mode fast

    U1STA = 0b1000010100010000;

    /// timer/////
    T1CON = 0xA200;
    PR1 = 0x257F;
    /// end timer////

    IFS0bits.T1IF = 0;

}

void UARTInit9600(void) {
    CLKDIV = 0x0000;
    //    TRISB = 0x0000;     // port B en sortie

    //U1BRG = 0x0010; pour mode fast
    U1BRG = 0x0067;

    //U1MODE = 0b1000100000001000; mode fast
    U1MODE = 0b1000100000000000;
    U1STA = 0b1000010100010000;

    /// timer/////
    T1CON = 0xA200;
    PR1 = 0x257F;
    /// end timer////

    IFS0bits.T1IF = 0;

}

void UARTBasicCommand() {
    UARTWriteChar('A');
    UARTWriteChar('T');
    UARTWriteChar('+');
}

void UARTWriteChar(char data) {
    while (U1STAbits.UTXBF == 1) {
    }
    U1TXREG = data;
}

void UARTWriteByte(UINT8_T data) {
    while (U1STAbits.UTXBF == 1) {
    }
    U1TXREG = data;
}


void UARTConfigBaudRate230400() {
    UARTBasicCommand();
    UARTWriteChar('B');
    UARTWriteChar('A');
    UARTWriteChar('U');
    UARTWriteChar('D');
    UARTWriteChar('9');
}

void UARTSetBTName() {
    UARTBasicCommand();
    UARTWriteChar('N');
    UARTWriteChar('A');
    UARTWriteChar('M');
    UARTWriteChar('E');
    UARTWriteChar('T');
    UARTWriteChar('w');
    UARTWriteChar('i');
    UARTWriteChar('n');
    UARTWriteChar('M');
    UARTWriteChar('a');
    UARTWriteChar('x');

}

void UARTSetNotification() {
    UARTBasicCommand();
    UARTWriteChar('N');
    UARTWriteChar('O');
    UARTWriteChar('T');
    UARTWriteChar('I');
    UARTWriteChar('1');
} // notify when connected

void UARTSetParity(){
    UARTBasicCommand();
    UARTWriteChar('C');
    UARTWriteChar('H');
    UARTWriteChar('K');
    UARTWriteChar('0');
}// no parity

void UARTSetStopBit(){
    UARTBasicCommand();
    UARTWriteChar('S');
    UARTWriteChar('T');
    UARTWriteChar('O');
    UARTWriteChar('P');
    UARTWriteChar('1');
} // one stop bit

void UARTSetDisc(){
    UARTBasicCommand();
    UARTWriteChar('D');
    UARTWriteChar('I');
    UARTWriteChar('S');
    UARTWriteChar('C');
    UARTWriteChar('0');
} // discoverable et connectable

void UARTsetMode() {
    UARTBasicCommand();
    UARTWriteChar('T');
    UARTWriteChar('Y');
    UARTWriteChar('P');
    UARTWriteChar('E');
    UARTWriteChar('0');
} // transmit mode

void UARTResetBTModule() {
    UARTBasicCommand();
    UARTWriteChar('D');
    UARTWriteChar('E');
    UARTWriteChar('F');
    UARTWriteChar('A');
    UARTWriteChar('U');
    UARTWriteChar('L');
    UARTWriteChar('T');
}


//Set up UART at 9600 and wake up BT + set BT baud to 230400 then put the pic uart to 230400 and wake module again

void initBTModule() {
    UARTInit230400();
    UARTResetBTModule();
    __delay_ms(500);
    UARTInit9600();
    UARTResetBTModule();
    __delay_ms(500);
    UARTSetBTName();
    __delay_ms(700);
    //UARTsetMode();
    //__delay_ms(100);
    UARTSetNotification(); 
    __delay_ms(100);
    //UARTSetParity(); 
    //__delay_ms(100);
    //UARTSetStopBit(); 
    //__delay_ms(100);
    //UARTSetDisc(); 
    //__delay_ms(100);
    UARTConfigBaudRate230400();
    __delay_ms(100);
    UARTInit230400();
    __delay_ms(100);
    if(U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
    }
    while(U1STAbits.URXDA == 1) {
        U1TXREG = U1RXREG;
    } 
}