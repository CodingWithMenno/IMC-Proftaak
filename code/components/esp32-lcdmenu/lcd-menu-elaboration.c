#include "lcd-menu.c"
#include "radioController.h"
#include "sdcard-mp3.h"
#include "goertzel.h"

/*
This file is to work out the onClicks, onExit, onEnter and update functions of the lcd-menu's. 

(This file is an extension of the lcd-menu.c file, so that is why there is a ".c" file included)
*/

//Main menu
void onClickMainEcho(i2c_lcd1602_info_t* lcd_info)
{
    displayMenu(lcd_info, ECHO_MENU_ID);
}

void onClickMainRadio(i2c_lcd1602_info_t* lcd_info)
{
    displayMenu(lcd_info, RADIO_MENU_ID);
}

void onClickMainClock(i2c_lcd1602_info_t* lcd_info)
{
    displayMenu(lcd_info, CLOCK_MENU_ID);
}

//Echo menu
void onClickEchoSpeech(i2c_lcd1602_info_t* lcd_info)
{
    displayMenu(lcd_info, SPEECH_MENU_ID);
}


//Radio menu
void onEnterRadio()
{
    printf("Entered the radio menu\n");
    xTaskCreate(&radio_task, "radio_task", 1024 * 3, NULL, 8, NULL);
}

void onExitRadio()
{
    printf("Exited the radio menu\n");
    radio_quit();
}

void onClickRadio538()
{
    radio_switch(lcdMenus[RADIO_MENU_ID].items[0].text);
}

void onClickRadioQ()
{
    radio_switch(lcdMenus[RADIO_MENU_ID].items[1].text);
}

void onClickRadioSky()
{
    radio_switch(lcdMenus[RADIO_MENU_ID].items[2].text);
}

//Klok menu
void onEnterClock()
{
    printf("Entered the radio menu\n");
    xTaskCreate(&mp3_task, "radio_task", 1024 * 3, NULL, 8, NULL);
}

void onExitClock()
{
    printf("Exited the radio menu\n");
    mp3_stopTask();
}

void onUpdateClock(void *p)
{
    strcpy(lcdMenus[CLOCK_MENU_ID].items[0].text, (char*) p);
}

//Speech menu
void onEnterSpeech()
{
    goertzel_start();
}

void onExitSpeech()
{
    goertzel_stop();
}