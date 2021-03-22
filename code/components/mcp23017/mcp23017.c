#include "mcp23017.h"
	
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include <string.h>

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

    while(config->task_enabled)
    {
        ESP_LOGI(TAG, "MCP23017 TASK RUNNING");
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