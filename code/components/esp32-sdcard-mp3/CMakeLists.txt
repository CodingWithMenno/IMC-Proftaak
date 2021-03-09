set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCS "sdcard-mp3.c")
set(COMPONENT_PRIV_REQUIRES "nvs_flash" "audio_pipeline" "audio_stream" "esp_peripherals")

register_component()
