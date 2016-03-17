#ifndef GLCD_H
#define	GLCD_H

#define CSLOW 0
#define CSHIGH 1
#define FCY 16000000UL

#include <p24FV16KM204.h>
#include <libpic30.h>

#define LCD_TRIS      TRISB
#define LCD_DATA      PORTB
#define ENABLE        LATCbits.LATC5
#define RW            LATCbits.LATC3
#define DI            LATCbits.LATC4
#define RESET         LATCbits.LATC2
#define CS1           LATCbits.LATC8
#define CS2           LATCbits.LATC9

#define LEFT          0b01
#define RIGHT         0b10
#define BOTH          0b11
#define NONE          0b00

// Ports d�finis pour couper le bluetooth
#define RTS LATBbits.LATB9   
#define RX LATCbits.LATC6
#define TX LATCbits.LATC7
#define CTS LATBbits.LATB8

void _lcd_enable(void);

unsigned char _lcd_status(void);

void _lcd_reset(void);

void _lcd_waitbusy(void);

void lcd_set_address(unsigned char y);

void lcd_set_page(unsigned char page);

void lcd_draw_bar(unsigned char index, unsigned char value, int isReference);

void lcd_screenon(int on);

void lcd_on();

void lcd_off();

void lcd_clear_screen(void);

void lcd_selectside(unsigned char sides);

void lcd_write(unsigned char data);

unsigned char lcd_read(void);

void lcd_plotpixel(unsigned char rx, unsigned char ry);

void lcd_putchar(char c);

void lcd_puts(char *string);

void lcd_bitmap(const char *bmp);

void lcd_char(char c);

void lcd_testChar(char c);

void lcd_longString(const char *string);

void lcd_charAt(char c, int x, int y);

void lcd_startLine(unsigned int z);

void lcd_horizontalBar(unsigned char index, unsigned char val);

void lcd_draw_bar(unsigned char index, unsigned char value, int handling);

void lcd_draw_reversed_char(unsigned char x, unsigned char y, char c);

void lcd_draw(unsigned char page, unsigned char y, unsigned char symbol);

void lcd_draw_char(unsigned char x, unsigned char y, char c);

#endif