#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp32/rom/uart.h"

#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"
#include "qwiic_twist.h"
#include "sdcard-mp3.h"
#include "radioController.h"


#define TAG "app"

// Undefine USE_STDIN if no stdin is available (e.g. no USB UART) - a fixed delay will occur instead of a wait for a keypress.
//#define USE_STDIN  1
#undef USE_STDIN

#define I2C_MASTER_NUM           I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN    0                     // disabled
#define I2C_MASTER_RX_BUF_LEN    0                     // disabled
#define I2C_MASTER_FREQ_HZ       100000
#define I2C_MASTER_SDA_IO        CONFIG_I2C_MASTER_SDA
#define I2C_MASTER_SCL_IO        CONFIG_I2C_MASTER_SCL
#define LCD_NUM_ROWS			 4
#define LCD_NUM_COLUMNS			 40
#define LCD_NUM_VIS_COLUMNS		 20

//lcd general settings
static i2c_lcd1602_info_t *lcd_info;

//rotary encoder functions
static void onEncoderClicked();
static void onEncoderPressed();
static void onEncoderMoved(int16_t);

//boolean to check if you went back a menu
static bool wentBack = false;
static int clickCounter = 0;


static void i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
}

// WARNING: ESP32 does not support blocking input from stdin yet, so this polls
// the UART and effectively hangs up the SDK.
static void wait(unsigned int time)
{
    vTaskDelay(time / portTICK_RATE_MS);
}

void radioInit()
{
    xTaskCreate(&radio_task, "radio_task", 1024 * 2, NULL, 8, NULL);
}

void i2cInit() 
{
    // Set up I2C
    i2c_master_init();
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = CONFIG_LCD1602_I2C_ADDRESS;
    

    // Set up the SMBus
    smbus_info_t *smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

    // Lcd and menu init
    lcd_info = i2c_lcd1602_malloc();
    i2c_lcd1602_init(lcd_info, smbus_info, true, LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VIS_COLUMNS);
    i2c_lcd1602_set_cursor(lcd_info, true);
    i2c_lcd1602_move_cursor(lcd_info, 4, 1);
    i2c_lcd1602_write_string(lcd_info, "Starting...");

    //Rotary init
    qwiic_twist_t *qwiic_info = (qwiic_twist_t*)malloc(sizeof(qwiic_twist_t));

    qwiic_info->smbus_info = smbus_info;
    qwiic_info->i2c_addr = QWIIC_TWIST_ADDRESS;
    qwiic_info->port = i2c_num;
    qwiic_info->xMutex = xSemaphoreCreateMutex();
    qwiic_info->task_enabled = true;
    qwiic_info->task_time = 0;
    qwiic_info->onButtonPressed = &onEncoderPressed;
    qwiic_info->onButtonClicked = &onEncoderClicked;
    qwiic_info->onMoved = &onEncoderMoved;
    
    qwiic_twist_init(qwiic_info);
    menu_initMenus(lcd_info);
    qwiic_twist_start_task(qwiic_info);
}

static void onEncoderPressed()
{
    clickCounter++;
    if(clickCounter == 5)
    {
        menu_goToParentMenu(lcd_info);
        clickCounter = 0;
        wentBack = true;
    }
}

static void onEncoderClicked()
{
    if (!wentBack)
    {
        menu_onClick(lcd_info);
    }
    clickCounter = 0;
    wentBack = false; 
}

static void onEncoderMoved(int16_t diff)
{
    if(diff>0)
    {
        menu_goToNextItem(lcd_info); 
    } else 
    {
        menu_goToPreviousitem(lcd_info);
    }
}

void app_main()
{
    i2cInit();
    // radioInit();

    // radio_switch("538");
    // wait(10000);
    // radio_switch("SKY");

    // mp3_load("/sdcard/test.mp3");
    // wait(1000);
    // wait(1000);
    // wait(1000);
    // mp3_load("/sdcard/test.mp3");

    while(1)
    {
        // mp3_update();
        // mp3_addToQueue("/sdcard/test.mp3");
        wait(5000);
    }

    // radio_stop();
}