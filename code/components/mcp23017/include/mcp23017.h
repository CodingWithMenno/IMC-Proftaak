#ifndef MCP23017_H
#define MCP23017_H

#include <stdbool.h>
#include "smbus.h"


#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

#define MCP23017_ADDRES 0x20

typedef struct
{
    smbus_info_t * smbus_info;
    uint8_t i2c_addr;
    i2c_port_t port;
    SemaphoreHandle_t xMutex;

    gpio_num_t sda_gpio;
    gpio_num_t scl_gpio;

    // Tasks settings
	bool task_enabled;
	uint16_t task_time;
} mcp23017_t;

typedef enum
{
    MCP23017_GPIO_OUTPUT = 0,
    MCP23017_GPIO_INPUT
} mcp23017_gpio_mode_t;

esp_err_t mcp23017_init(mcp23017_t* config);

bool mcp23017_conntected(mcp23017_t* config);

esp_err_t mcp23017_start_task(mcp23017_t* config);

esp_err_t mcp23017_stop_task(mcp23017_t* config);

esp_err_t mcp23017_set_mode(mcp23017_t *dev, uint8_t pin, mcp23017_gpio_mode_t mode);

esp_err_t mcp23017_set_level(mcp23017_t *dev, uint8_t pin, uint32_t val);

#endif

