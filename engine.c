#include <p24FV16KM204.h>


#include "engine.h"
#ifndef LOGO_H
#include "logo.h"
#endif
#include "timer.h"
#include "FRENCH.h"
#include "glcd.h"
#include "twinmaxUI.h"
#include "params.h"
#include "inputs.h"
#include "headers.h"


enum engine_phase{
    INIT,
    RUN,
    SLEEP
};

volatile int is_blueetooth_enable;
volatile enum engine_phase phase = INIT;




void __attribute__((__interrupt__, __auto_psv__)) _CNInterrupt(void){

    delay_ms(100);
    
    if(LEFT_BUTTON == 1){
        button_left_interupt();
    }else if(RIGHT_BUTTON == 1){
        button_right_interupt();
    }else if(SELECTION_BUTTON == 1){
        button_select_interupt();
    }else if(POWER_BUTTON == 1){
        button_power_interupt();
    }else if(BACKLIGHT_BUTTON == 1){
        button_light_interupt();
    }

    delay_ms(100);
        // Reset flag
    IFS1bits.CNIF = 0;
   

    ;

    
    return;
}

void init_button_interrupt(){
    /*
    * INTCON1, INTCON2 Registers
    * IFSn: Interrupt Flag Status Registers
    * IECn: Interrupt Enable Control Registers
    * IPCn: Interrupt Priority Control Registers
    */
    
    // Use standard vector table
    // DISI is not active
    // Every Interrupts on positive edge
    INTCON2 = 0;
    
    // Interrupt Nesting Disabled
    INTCON1bits.NSTDIS = 0;
    
    //Reset all flags
    IFS0 = 0;
    IFS1 = 0;
    IFS2 = 0;
    IFS3 = 0;
    IFS4 = 0;
    IFS5 = 0;
    IFS6 = 0;
    
    // Enable Change Notification(CN) Interrupts.
    IEC1bits.CNIE = 1;

    /*
     * The CNEN1 and CNEN3 registers contain the interrupt enable control
     * 
     * RA9 = CN34 
     * RA10 = CN35
     * RA11 = CN36
     * RA2 = CN30   => power button 
     * RA3 = CN29
     * 
    */
    
    CNEN1 = 0b0000000000000000; 
    CNEN2 = 0b0110000000000000;
    CNEN3 = 0b0000000000011100; 
    
    
}

void set_scale(unsigned short new_reference, unsigned short new_range){
    extern unsigned short pression_range;
    extern unsigned short pression_reference;

    // todo : julien 15/02/2016
    //can't step inside the next if because we are working with unsigned shit.
    // need to use somthing else to handle negative values
    if(new_reference - new_range / 2 < 0){
        new_reference = new_range / 2;
    }

    pression_reference = new_reference;

    if(new_range <= MAX_RANGE && new_range >= MIN_RANGE){
        pression_range = new_range;
    }
    return;
}

void button_left_interupt(){
    extern unsigned short reference_sensor;
    extern unsigned short weightedAverages[4];
    extern unsigned short pression_range;
    set_scale(weightedAverages[reference_sensor], pression_range * RANGE_STEP);
    delay_ms(200);
    return;
}

void button_right_interupt(){
    extern unsigned short reference_sensor;
    extern unsigned short weightedAverages[4];
    extern unsigned short pression_range;
    set_scale(weightedAverages[reference_sensor], pression_range / RANGE_STEP);
    delay_ms(200);
    return;
}

void button_select_interupt(){
    extern unsigned short reference_sensor;
    extern unsigned short weightedAverages[4];
    extern unsigned short pression_range;
    set_scale(weightedAverages[reference_sensor], pression_range);
    delay_ms(200);
    return;
}

void button_light_interupt(){
    BACKLIGHT_OUTPUT ^= 1; // pwm
    return;
}

void button_power_interupt(){
     if(phase == SLEEP){
         phase = INIT;
     }else{
         //TODO changer les registres ECN pour désactiver les interruptions autres que le buton pwoer
         CNEN1 =0;
         CNEN2 = 0b0100000000000000; // Only keep power button interrupt
         CNEN3 = 0 ;         
         phase = SLEEP;
         timer_stop();
         lcd_clear_screen();
         tui_write_at(3,40,BYE,0,0);
         delay_ms(2000);
//         PORTA = 0;
//         PORTB = 0;
//         LATC = 0;
         //lcd_off();
         POWER_CIRCUIT_ENABLE = 0;
         IFS1bits.CNIF = 0;   
         
         
         //OSCCONbits.CLKLOCK;

         Sleep();
        }
    return;
}

void engine_splash(){
    lcd_on();
    lcd_clear_screen();
    lcd_on();
    lcd_bitmap(twinmaxLogo);    
    delay_ms(4000);
    lcd_clear_screen();
    lcd_on();
}

void engine_display_bluetooth_question(){
    tui_write_at(1, 15, BLUETOOTH, 0, 0);
    tui_write_at(5, 10, YES, is_blueetooth_enable==0, 0);
    tui_write_at(5, 90, NO, is_blueetooth_enable==1, 0);
}

void engine_display_reference_question(int reference_sensor){
    tui_write_at(1, 15, REFERENCE, 0, 0);
    tui_write_at(5, 5, "1", reference_sensor==0, 0);
    tui_write_at(5, 35, "2", reference_sensor==1, 0);
#ifndef TWO_BARS
    tui_write_at(5, 65, "3", reference_sensor==2, 0);
    tui_write_at(5, 95, "4", reference_sensor==3, 0); 
#endif
}

int engine_ask_for_bluetooth(){
    int selected = 0;
    
    while(selected == 0){
        engine_display_bluetooth_question(is_blueetooth_enable);
        while((LEFT_BUTTON || SELECTION_BUTTON || RIGHT_BUTTON) == 0){  
        }
        if(LEFT_BUTTON == 1){
            is_blueetooth_enable = (is_blueetooth_enable + 1) % 2;
        }else if(RIGHT_BUTTON == 1){
            is_blueetooth_enable = (is_blueetooth_enable + 1) % 2;
        }else{
            selected = 1;
        }
        delay_ms(200);
    }

    return is_blueetooth_enable;
}

int engine_ask_for_reference_sensor(){
    int selected = 0;
    unsigned int reference = 0;

    while(selected == 0){
        engine_display_reference_question(reference);
        while((LEFT_BUTTON || RIGHT_BUTTON || SELECTION_BUTTON) == 0){
        }
        if (LEFT_BUTTON == 1){
            reference = (reference - 1) % 4;            
        }else if(RIGHT_BUTTON == 1){
            reference = (reference + 1) % 4;
        }else if(SELECTION_BUTTON == 1){
            selected = 1;
        }
        delay_ms(200);
    }
    return (int)reference;
}

void engine_menu(){ 
    //TODO (julien 10/02/2016) don't disable all interruption because we want to be interrupted if user press sleep button
    
    // Temporaly disable button interruption
    IEC1bits.CNIE = 0;
    extern unsigned short reference_sensor;

     if(engine_ask_for_bluetooth() == 1){
         ble_init();
     }

     lcd_clear_screen();
     reference_sensor = engine_ask_for_reference_sensor();

    lcd_clear_screen();
}

void engine_initialization() {
   
    POWER_CIRCUIT_ENABLE = 1 ; //ALMIENTATION ENABLE
    engine_splash();    
    // Initialise sleeping options
    RCONbits.RETEN = 1;
    RCONbits.PMSLP = 0;
    
    
    
    BACKLIGHT_OUTPUT = 0; // pwm
    engine_menu();
    init_button_interrupt();
    averages_init();
    adc_init();
    lcd_clear_screen();
    
    engine_start();
}

void engine_start() {
    extern unsigned short weightedAverages[4];
    extern unsigned short pression_range;
    extern unsigned short pression_reference;
    extern unsigned short reference_sensor;    
    pression_range = MAX_RANGE;             
    unsigned short testVals[4];
    int i = 0;
    phase = RUN;
    
    timer_start();

    for(i=0;i<10;i++){
        average_update_weighted_averages();
        delay_ms(10)
    };

    average_update_weighted_averages();
    pression_reference = weightedAverages[reference_sensor];
    
    while(phase == RUN){
        average_update_weighted_averages();    
        tui_displayMeasures(weightedAverages, pression_reference, pression_range, reference_sensor);
        delay_ms(100);
    }
}
