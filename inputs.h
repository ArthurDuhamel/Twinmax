#ifndef INPUTS_H
#define	INPUTS_H

/*BUTTONS*/
#define RIGHT_BUTTON PORTAbits.RA9
#define LEFT_BUTTON PORTAbits.RA11
#define SELECTION_BUTTON PORTAbits.RA10
#define POWER_BUTTON PORTAbits.RA2
#define BACKLIGHT_BUTTON PORTAbits.RA3

/* ADC BUFFERS */
/* NB : les buffers et les capteurs semblent inversés pour des raisons
 * d'affichage des barres à l'écran, le capteur 1 étant à gauche sur le 
 * prototype.
 */
#define SENSOR1BUF       ADC1BUF12
#define SENSOR1AVGBUF    ADC1BUF11
#define SENSOR2BUF       ADC1BUF10
#define SENSOR2AVGBUF    ADC1BUF9
#define SENSOR3BUF       ADC1BUF0
#define SENSOR3AVGBUF    ADC1BUF1
#define SENSOR4BUF       ADC1BUF6
#define SENSOR4AVGBUF    ADC1BUF16
#define BATTERYLEVEL     ADC1BUF7

/* OUTPUTS */
#define POWER_CIRCUIT_ENABLE PORTAbits.RA8 
#define BACKLIGHT_OUTPUT PORTAbits.RA7

// For BLE :
#define RTSoff PORTBbits.RB9   
#define RXoff PORTCbits.RC6
#define TXoff PORTCbits.RC7
#define CTSoff PORTBbits.RB8

#endif	/* INPUTS_H */
