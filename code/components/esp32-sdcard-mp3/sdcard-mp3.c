#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "fatfs_stream.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"

#include "esp_peripherals.h"
#include "periph_sdcard.h"
#include "periph_touch.h"
#include "audio_hal.h"
#include "board.h"
#include "sdcard-mp3.h"

//Static functions
static FILE *get_file(int);
static int my_sdcard_read_cb(audio_element_handle_t , char*, int, TickType_t , void*);

audio_pipeline_handle_t pipeline;
audio_element_handle_t i2s_stream_writer, mp3_decoder;
audio_event_iface_handle_t evt;

static const char *TAG = "SDCARD_MP3_CONTROL_EXAMPLE";

static FILE *get_file(int next_file)
{
    static FILE *file;
    static int file_index = 0;

    if (next_file != CURRENT) 
    {
        // advance to the next file
        if (++file_index > MP3_FILE_COUNT - 1) 
        {
            file_index = 0;
        }
        if (file != NULL) 
        {
            fclose(file);
            file = NULL;
        }
        
    }
    // return a handle to the current file
    if (file == NULL) 
    {
        file = fopen(mp3_file[file_index], "r");
        if (!file) 
        {
            printf("Error opening file");
            return NULL;
        }
    }
    return file;
}

static int my_sdcard_read_cb(audio_element_handle_t el, char *buf, int len, TickType_t wait_time, void *ctx)
{
    int read_len = fread(buf, 1, len, get_file(CURRENT));
    if (read_len == 0)
        read_len = AEL_IO_DONE;
    return read_len;
}

void mp3_init()
{
	//audio_pipeline_handle_t pipeline;
    //audio_element_handle_t i2s_stream_writer, mp3_decoder;

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGI(TAG, "[1.0] Initialize peripherals management");
    esp_periph_config_t periph_cfg = { 0 };
    esp_periph_init(&periph_cfg);

    ESP_LOGI(TAG, "[1.1] Start SD card peripheral");
    periph_sdcard_cfg_t sdcard_cfg = 
    {
        .root = "/sdcard",
        .card_detect_pin = SD_CARD_INTR_GPIO, //GPIO_NUM_34
    };
    esp_periph_handle_t sdcard_handle = periph_sdcard_init(&sdcard_cfg);
    ESP_LOGI(TAG, "[1.2] Start SD card peripheral");
    esp_periph_start(sdcard_handle);

    // Wait until sdcard was mounted
    while (!periph_sdcard_is_mounted(sdcard_handle))
        vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "[1.3] Initialize Touch peripheral");
    periph_touch_cfg_t touch_cfg = 
    {
        .touch_mask = TOUCH_SEL_SET | TOUCH_SEL_PLAY | TOUCH_SEL_VOLUP | TOUCH_SEL_VOLDWN,
        .tap_threshold_percent = 70,
    };
    esp_periph_handle_t touch_periph = periph_touch_init(&touch_cfg);

    ESP_LOGI(TAG, "[1.4] Start touch peripheral");
    esp_periph_start(touch_periph);

    ESP_LOGI(TAG, "[ 2 ] Start codec chip");
    audio_hal_codec_config_t audio_hal_codec_cfg =  AUDIO_HAL_ES8388_DEFAULT();
    audio_hal_codec_cfg.i2s_iface.samples = AUDIO_HAL_44K_SAMPLES;
    audio_hal_handle_t hal = audio_hal_init(&audio_hal_codec_cfg, 0);
    audio_hal_ctrl_codec(hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);
    int player_volume;
    audio_hal_get_volume(hal, &player_volume);

    ESP_LOGI(TAG, "[3.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    ESP_LOGI(TAG, "[3.1] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[3.2] Create mp3 decoder to decode mp3 file");
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_decoder = mp3_decoder_init(&mp3_cfg);
    audio_element_set_read_cb(mp3_decoder, my_sdcard_read_cb, NULL);

    ESP_LOGI(TAG, "[3.3] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, mp3_decoder, "mp3");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    ESP_LOGI(TAG, "[3.4] Link it together [my_sdcard_read_cb]-->mp3_decoder-->i2s_stream-->[codec_chip]");
    audio_pipeline_link(pipeline, (const char *[]) {"mp3", "i2s"}, 2);

    ESP_LOGI(TAG, "[4.0] Setup event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[4.1] Listen for all pipeline events");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[4.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_get_event_iface(), evt);

    ESP_LOGW(TAG, "[ 5 ] Tap touch buttons to control music player:");
    ESP_LOGW(TAG, "      [Play] to start, pause and resume, [Set] next song.");
    ESP_LOGW(TAG, "      [Vol-] or [Vol+] to adjust volume.");
}

void mp3_update()
{
	/* Handle event interface messages from pipeline
		to set music info and to advance to the next song
	*/
	audio_event_iface_msg_t msg;
	esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
	if (ret != ESP_OK) 
    {
		ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
		return;
	}
	if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT) 
    {
		// Set music info for a new song to be played
		if (msg.source == (void *) mp3_decoder
			&& msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) 
            {
			audio_element_info_t music_info = {0};
			audio_element_getinfo(mp3_decoder, &music_info);
			ESP_LOGI(TAG, "[ * ] Received music info from mp3 decoder, sample_rates=%d, bits=%d, ch=%d",
						music_info.sample_rates, music_info.bits, music_info.channels);
			audio_element_setinfo(i2s_stream_writer, &music_info);
			i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
			return;
		}
		// Advance to the next song when previous finishes
		if (msg.source == (void *) i2s_stream_writer
			&& msg.cmd == AEL_MSG_CMD_REPORT_STATUS) 
            {
			audio_element_state_t el_state = audio_element_get_state(i2s_stream_writer);
			if (el_state == AEL_STATE_FINISHED) 
            {
				ESP_LOGI(TAG, "[ * ] Finished, advancing to the next song");
				audio_pipeline_stop(pipeline);
				audio_pipeline_wait_for_stop(pipeline);
				get_file(NEXT);
				audio_pipeline_run(pipeline);
			}
		}
	}
}

void mp3_start()
{
	audio_pipeline_run(pipeline);
}

void mp3_stop()
{
	ESP_LOGI(TAG, "[ 6 ] Stop audio_pipeline");
    audio_pipeline_terminate(pipeline);

    audio_pipeline_unregister(pipeline, mp3_decoder);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);

    /* Terminate the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);

    /* Stop all peripherals before removing the listener */
    esp_periph_stop_all();
    audio_event_iface_remove_listener(esp_periph_get_event_iface(), evt);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    audio_pipeline_deinit(pipeline);
    audio_element_deinit(i2s_stream_writer);
    audio_element_deinit(mp3_decoder);
    esp_periph_destroy()
}