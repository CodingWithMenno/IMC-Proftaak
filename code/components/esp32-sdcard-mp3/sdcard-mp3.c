#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "fatfs_stream.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"

#include "esp_peripherals.h"
#include "periph_sdcard.h"
#include "board.h"
#include "sdcard-mp3.h"
#include "mp3_queue.h"

static const char *TAG = "SDCARD_MP3_EXAMPLE";

static void init();
static void update();
static void reset();

SemaphoreHandle_t mp3Mutex;
Queue *playlist;
static int isRunning = 0;
static int isPlaying = 0;
static int isInit = 0;

audio_pipeline_handle_t pipeline;
audio_element_handle_t fatfs_stream_reader, i2s_stream_writer, mp3_decoder;
audio_event_iface_handle_t evt;
esp_periph_set_handle_t set;

void mp3_task(void *p)
{
    init("/sdcard/test.mp3");
    mp3Mutex = xSemaphoreCreateMutex();
    playlist = NULL;

    isRunning = 1;
    while (isRunning)
    {
        xSemaphoreTake(mp3Mutex, portMAX_DELAY);

        if (!isPlaying && playlist != NULL)
        {
            char *audioFile = front(&playlist);
            if (audioFile != NULL)
            {
                mp3_play(audioFile);
            }
            
        }

        update();

        xSemaphoreGive(mp3Mutex);
        vTaskDelay(50 / portTICK_RATE_MS);
    }
    
    // reset();
    mp3_stop();
    freeQueue(&playlist);
    vTaskDelete(NULL);
}

void mp3_addToQueue(char *fileName)
{
    if (!isRunning)
        return;

    xSemaphoreTake(mp3Mutex, portMAX_DELAY);
    enqueue(&playlist, fileName);
    xSemaphoreGive(mp3Mutex);
}

void mp3_stopTask()
{
    if (!isRunning)
        return;

    xSemaphoreTake(mp3Mutex, portMAX_DELAY);
    isRunning = 0;
    xSemaphoreGive(mp3Mutex);
}

void mp3_play(char* fileName)
{
    if (isRunning)
    {
        reset();
        audio_element_set_uri(fatfs_stream_reader, fileName);
    } else
    {
        init(fileName);
    }

    ESP_LOGI(TAG, "[ 5 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);
    isPlaying = 1;
}

static void update()
{
    if (!isPlaying)
        return;

    audio_event_iface_msg_t msg;
    esp_err_t ret = audio_event_iface_listen(evt, &msg, 200);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
        return;
    }

    if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
        && msg.source == (void *) mp3_decoder
        && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
        audio_element_info_t music_info = {0};
        audio_element_getinfo(mp3_decoder, &music_info);

        ESP_LOGI(TAG, "[ * ] Receive music info from mp3 decoder, sample_rates=%d, bits=%d, ch=%d",
                    music_info.sample_rates, music_info.bits, music_info.channels);

        audio_element_setinfo(i2s_stream_writer, &music_info);
        i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
        return;
    }

    /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
    if ((int)msg.data == AEL_STATUS_STATE_FINISHED) 
    {
        vTaskDelay(500/portTICK_RATE_MS);
        ESP_LOGW(TAG, "[ * ] Stop event received");
        reset();
    }
}

static void reset()
{
    if (!isPlaying)
        return;

    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_element_reset_state(mp3_decoder);
    audio_element_reset_state(i2s_stream_writer);
    audio_pipeline_reset_ringbuffer(pipeline);
    audio_pipeline_reset_items_state(pipeline);
    isPlaying = 0;
}

static void init(char *fileName)
{
    if (isPlaying)
        mp3_stop();

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_INFO);

    if (!isInit)
    {
        ESP_LOGI(TAG, "[ 1 ] Mount sdcard");
        // Initialize peripherals management
        esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
        set = esp_periph_set_init(&periph_cfg);

        // Initialize SD Card peripheral
        audio_board_sdcard_init(set, SD_MODE_1_LINE);
    }

    ESP_LOGI(TAG, "[3.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    ESP_LOGI(TAG, "[3.1] Create fatfs stream to read data from sdcard");
    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_READER;
    fatfs_stream_reader = fatfs_stream_init(&fatfs_cfg);

    ESP_LOGI(TAG, "[3.2] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[3.3] Create mp3 decoder to decode mp3 file");
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_decoder = mp3_decoder_init(&mp3_cfg);

    ESP_LOGI(TAG, "[3.4] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, fatfs_stream_reader, "file");
    audio_pipeline_register(pipeline, mp3_decoder, "mp3");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    ESP_LOGI(TAG, "[3.5] Link it together [sdcard]-->fatfs_stream-->mp3_decoder-->i2s_stream-->[codec_chip]");
    audio_pipeline_link(pipeline, (const char *[]) {"file", "mp3", "i2s"}, 3);

    ESP_LOGI(TAG, "[3.6] Set up  uri (file as fatfs_stream, mp3 as mp3 decoder, and default output is i2s)");
    audio_element_set_uri(fatfs_stream_reader, fileName);

    ESP_LOGI(TAG, "[ 4 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[4.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[4.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

    isInit = 1;
}

void mp3_stop()
{
    if (!isInit)
        return;

    ESP_LOGI(TAG, "[ 7 ] Stop audio_pipeline");
    audio_pipeline_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    audio_pipeline_unregister(pipeline, fatfs_stream_reader);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);
    audio_pipeline_unregister(pipeline, mp3_decoder);

    /* Terminal the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);

    /* Stop all periph before removing the listener */
    esp_periph_set_stop_all(set);
    audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    audio_pipeline_deinit(pipeline);
    audio_element_deinit(fatfs_stream_reader);
    audio_element_deinit(i2s_stream_writer);
    audio_element_deinit(mp3_decoder);
    // esp_periph_set_destroy(set);

    isPlaying = 0;
}