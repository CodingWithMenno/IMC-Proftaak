#ifndef LCDMENUELAB_H
#define LCDMENUELAB_H

void onClickMainEcho(i2c_lcd1602_info_t*);
void onClickMainRadio(i2c_lcd1602_info_t*);
void onClickMainClock(i2c_lcd1602_info_t*);

void onClickEchoSpeech(i2c_lcd1602_info_t*);

void onEnterRadio();
void onExitRadio();
void onClickRadio538();
void onClickRadioQ();
void onClickRadioSky();

void onEnterClock();
void onUpdateClock(void*);
void onExitClock();

void onEnterSpeech();
void onExitSpeech();

#endif