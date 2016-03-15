#ifndef TWINMAX_UI_H
#define	TWINMAX_UI_H

#include "glcd.h"
#include <stdio.h>

#ifndef MYFONT_H
#define	MYFONT_H
#include "myFont.h"
#endif

#define FONT_WIDTH 6
#define NUMBER_WIDTH 4

void tui_drawGraph(unsigned char values[4], int referenceIndex);

void tui_write_at(unsigned char x,unsigned char y,const char* string,int reversed,int width);

void tui_menuItem(int index,const char *string,unsigned char highlighted);

void tui_displayMeasures(unsigned short measures[4],unsigned short referenceMeasure, unsigned short sensitivity, int referenceIndex);

void tui_numberAt(unsigned char x,unsigned char y,unsigned short val);

void glcd_smallNumberAt(unsigned char page,unsigned char y,unsigned char value,int reversed);

void tui_test();

unsigned short measure_to_pressure(short val);

void tui_battery();

void tui_draw_number(unsigned char page, unsigned char y, unsigned short val);

#endif
