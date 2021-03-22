#include "mcp23017.h"
	
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include <string.h>
#include <stdio.h>
#include <driver/i2c.h>

#define REG_IODIRA   0x00
#define REG_IODIRB   0x01
#define REG_IPOLA    0x02
#define REG_IPOLB    0x03
#define REG_GPINTENA 0x04
#define REG_GPINTENB 0x05
#define REG_DEFVALA  0x06
#define REG_DEFVALB  0x07
#define REG_INTCONA  0x08
#define REG_INTCONB  0x09
#define REG_IOCON    0x0A
#define REG_GPPUA    0x0C
#define REG_GPPUB    0x0D
#define REG_INTFA    0x0E
#define REG_INTFB    0x0F
#define REG_INTCAPA  0x10
#define REG_INTCAPB  0x11
#define REG_GPIOA    0x12
#define REG_GPIOB    0x13
#define REG_OLATA    0x14
#define REG_OLATB    0x15

#define CHECK(x) do { esp_err_t __; if ((__ = x) != ESP_OK) return __; } while (0)
#define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)
#define BV(x) (1 << (x))

static const char *TAG = "MCP23017";

void mcp23017_task(void* pvParameters);

esp_err_t mcp23017_init(mcp23017_t* config)
{
    // Set up the SMBus
    config->smbus_info = smbus_malloc();
    smbus_init(config->smbus_info, config->port, config->i2c_addr);
    smbus_set_timeout(config->smbus_info, 1000 / portTICK_RATE_MS);
    i2c_set_timeout(I2C_NUM_0, 20000);

    // Initialize Mutex
    config->xMutex = xSemaphoreCreateMutex();
    return ESP_OK;
}

bool mcp23017_connected(mcp23017_t* config)
{
    return true;
}

void mcp23017_task(void* pvParameters)
{
    mcp23017_t* config = (mcp23017_t*)pvParameters;

    uint8_t result = 0;
	esp_err_t err = 0;
    uint8_t *val = 0;

    //mcp23017_get_level(config, 1, val);
    //printf("WAARDE VAN PIN: %c", *val);

    while(config->task_enabled)
    {
        mcp23017_get_level(config, 1, val);
        //ESP_LOGI(TAG, "%d", val);
        //printf("WAARDE VAN PIN: %u", *val);
        
        
	   //ESP_LOGI(TAG, "Running");

        vTaskDelay(config->task_time / portTICK_RATE_MS);
    }
}

esp_err_t mcp23017_start_task(mcp23017_t* config) {
	config->task_enabled = true;
	if (config->task_time == 0) {
		config->task_time = 100;
	}
	ESP_LOGI(TAG, "Starting task");
	xTaskCreate(mcp23017_task, "mcp23017_task", 1024 * 2, (void*)config, 10, NULL);
			
	return ESP_OK;
}

esp_err_t mcp23017_stop_task(mcp23017_t* config) {
	config->task_enabled = false;
	
	ESP_LOGI(TAG, "Stopping task");
	return ESP_OK;
}

esp_err_t mcp23017_set_mode(mcp23017_t *dev, uint8_t pin, mcp23017_gpio_mode_t mode)
{
    return smbus_write_byte(dev->smbus_info, pin, mode);
}

esp_err_t mcp23017_set_level(mcp23017_t *dev, uint8_t pin, uint32_t val)
{
    smbus_write_byte(dev->smbus_info, 0x12, val);
    return smbus_write_byte(dev->smbus_info, 0x12, pin);
}



esp_err_t mcp23017_get_level(mcp23017_t *dev, uint8_t pin, uint8_t *val)
{
    CHECK_ARG(val);

    //bool buf;

    smbus_read_byte(dev->smbus_info, 0x12, val);
    printf("Dit werkt wel");
    
    //printf("WAARDE VAN PIN: %u", *val);
    //CHECK(smbus_read_byte(dev, 0x12, val));
    //smbus_read_byte(dev->smbus_info, 0x12, *val);
    //*val = buf ? 1 : 0;
    //ESP_LOGI(TAG, "%d");

    return ESP_OK;
}

// esp_err_t mcp23017_port_set_interrupt(mcp23017_t *dev, uint16_t mask, mcp23017_gpio_intr_t intr)
// {
//     CHECK_ARG(dev);

//     uint16_t int_en;
//     CHECK(smbus_read_byte(dev, REG_GPINTENA, &int_en));

//     if(intr == MCP23017_INT_DISABLED)
//     {
//         // disable interrupts
//         int_en &= ~mask;
//         CHECK(smbus_write_byte(dev, REG_GPINTENA, int_en));

//         return ESP_OK;
//     }

//     uint16_t int_con;
//     CHECK(smbus_read_byte(dev, REG_GPINTENA, &int_con));

//     if(intr == MCP23017_INT_ANY_EDGE)
//         int_con &= ~mask;
//     else
//     {
//         int_con |= mask;

//         uint16_t int_def;
//         CHECK(smbus_read_byte(dev, REG_DEFVALA, &int_def));
//         if(intr == MCP23017_INT_LOW_EDGE)
//             int_def |= mask;
//         else   
//             int_def &= ~mask;
//         CHECK(write_reg_16(dev, REG_DEFVALA, int_def));
//     }

//     CHECK(write_reg_16(dev, REG_INTCONA, int_con));

//     // enable interrupts
//     int_en |= mask;
//     CHECK(write_reg_16(dev, REG_GPINTENA, int_en));

//     return ESP_OK;
// }

// esp_err_t mcp23017_set_interrupt(mcp23017_t *dev, uint8_t pin, mcp23017_gpio_intr_t intr)
// {
//     return mcp23017_port_set_interrupt(dev, BV(pin), intr);
// }