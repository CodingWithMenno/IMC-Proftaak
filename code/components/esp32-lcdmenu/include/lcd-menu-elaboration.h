#ifndef LCDMENUELAB_H
#define LCDMENUELAB_H

void onClickMainEcho(i2c_lcd1602_info_t*);
void onClickMainRadio(i2c_lcd1602_info_t*);
void onClickMainClock(i2c_lcd1602_info_t*);

void onEnterRadio();
void onExitRadio();

void onEnterClock();
void onUpdateClock(void*);
void onExitClock();

#endif