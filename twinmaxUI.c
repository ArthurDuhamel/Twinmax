#include "twinmaxUI.h" 
#include "params.h"
#include "engine.h"
#include "glcd.h"
//#ifndef LOGO_H
//#include "logo.h"
//#endif

const char battery[6] = {
    0b11111110,
    0b10000010,
    0b10000011,
    0b10000011,
    0b10000010,
    0b11111110
};

void tui_draw_graph(unsigned char height[4], int referenceIndex) {
    lcd_draw_bar(0, height[0], referenceIndex == 0);
    lcd_draw_bar(1, height[1], referenceIndex == 1);
#ifndef TWO_BARS
    lcd_draw_bar(2, height[2], referenceIndex == 2);
    lcd_draw_bar(3, height[3], referenceIndex == 3);
#endif
}

void tui_write_at(unsigned char page, unsigned char y, const char* string, int reversed, int width) {
    int i = 0;
    unsigned char referenceY = y;
    if (width == 0) {
        width = 127;
    }
    while (string[i] != 0) {
        //start new line if address is at the end of the screen
        if (y + FONT_WIDTH > width) {
            y = referenceY;
            page++;
        }
        if (reversed == 1) {
            lcd_draw_reversed_char(page, y, string[i++]);
            y += FONT_WIDTH;
        } else {
            lcd_draw_char(page, y, string[i++]);
            y += FONT_WIDTH;
        }
    }
}

unsigned short measure_to_pressure(short measure) {
    double pressure;
    pressure = 0.54541 * measure - 138;
    return (unsigned short) pressure;
}

void tui_displayMeasures(unsigned short measures[4], unsigned short reference, unsigned short range, int referenceIndex) {
    unsigned char height[4];
    int i = 0;
    long temp = 0;

    for (i = 0; i < 4; i++) {
        // Calculate each height between 0 and 64 to display
        temp = measures[i];
        temp = temp - reference;
        temp = temp + range / 2;
        temp = 63 * temp;
        temp = temp / range;

        // Set to 63 or 0 if the value is out of range
        if (temp >= 63) {
            temp = 63;
        }
        if (temp <= 0) {
            temp = 0;
        }
        // store the value for display
        height[i] = (unsigned char) temp;
    }

    // draw the graph with the height
    tui_draw_graph(height, referenceIndex);

    // draw the scale on the left
    tui_draw_number(0, 0, measure_to_pressure(reference + range / 2));
    tui_draw_number(3, 0, measure_to_pressure(reference));
    tui_draw_number(7, 0, measure_to_pressure((reference - range / 2)));
}

void tui_draw_number(unsigned char page, unsigned char y, unsigned short val) {
    char string[5];
    // Convert an unsigned short to a 5 char string
    snprintf(string, 5, "%u", val);
    // Clear the space
    tui_write_at(page, y, "     ", 0, 0);
    // Draw the 5 char string
    tui_write_at(page, y, string, 0, 0);
}

void tui_battery(unsigned short val) {
    /*
    int i = 0;
    unsigned char t;
    unsigned char fillMeter = 0b11111100; //<< (5 - val / 20);
    lcd_draw(0, 120, battery[i]);
    for (i = 1; i < 6; i++) {
        t = (unsigned char) (battery[i] | fillMeter);
        lcd_write(t);
    }
    */
    int i = 0;
    unsigned char t;
    unsigned short lvl = ((val - 1638)/160)+1;
    unsigned char fillMeter = 0b11111100 << (5 - lvl);
    lcd_draw(0, 120, battery[i]);
    for (i = 1; i < 6; i++) {
        t = (unsigned char) (battery[i] | fillMeter);
        lcd_write(t);
    }
}