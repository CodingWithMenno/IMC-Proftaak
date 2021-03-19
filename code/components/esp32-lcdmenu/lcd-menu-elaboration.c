#include "lcd-menu.c"
#include "radioController.h"
#include "sdcard-mp3.h"
#include "talking_clock.h"
#include <sys/time.h>

// Clock timer
TimerHandle_t timer_1_sec;
void timer_1_sec_callback(TimerHandle_t xTimer);

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
    printf("Entered the clock menu\n");
    xTaskCreate(&mp3_task, "radio_task", 1024 * 3, NULL, 8, NULL);

    vTaskDelay(2000/portTICK_RATE_MS);

    talking_clock_fill_queue();
    // timer_1_sec = xTimerCreate("MyTimer", pdMS_TO_TICKS(1000), pdTRUE, (void *)1, &timer_1_sec_callback);
    // if (xTimerStart(timer_1_sec, 10) != pdPASS)
    // {
    //     printf("Cannot start 1 second timer");
    // }
}

void onExitClock()
{
    printf("Exited the clock menu\n");
    mp3_stopTask();
}

void onUpdateClock(void *p)
{
    strcpy(lcdMenus[CLOCK_MENU_ID].items[0].text, (char*) p);
}

void onClickClockItem()
{
    mp3_addToQueue("/sdcard/test1.mp3");
    mp3_addToQueue("/sdcard/test2.mp3");
    mp3_addToQueue("/sdcard/test1.mp3");
}

void timer_1_sec_callback(TimerHandle_t xTimer)
{
    // Print current time to the screen
    // time_t now;
    // struct tm timeinfo;
    // time(&now);

    // char strftime_buf[20];
    // localtime_r(&now, &timeinfo);
    // sprintf(&strftime_buf[0], "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    // men((char*)"test");
    printf("test voor print");
    // size_t timeSize = strlen(strftime_buf);
    // if (menu_getCurrentMenuID() == 0)
    // {
    //     i2c_lcd1602_move_cursor(lcd_info, 6, 1);
    //     for (int i = 0; i < timeSize; i++)
    //     {
    //         i2c_lcd1602_write_char(lcd_info, strftime_buf[i]);
    //     }
    // }

    // Say the time every hour
    // if (timeinfo.tm_sec == 0 && timeinfo.tm_min % 10 == 0)
    // {
    //     talking_clock_fill_queue();
    // }
}