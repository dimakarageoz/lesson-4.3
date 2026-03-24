#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"
#include "driver/i2c_types.h"

#include "esp_err.h"

#include "ssd1306.h"

#include "rtc.h"

#define I2C_MASTER_SCL_IO           9
#define I2C_MASTER_SDA_IO           8
#define I2C_MASTER_RST_PIN          12

static char *DAYS[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

i2c_master_dev_handle_t* connectRTC(i2c_master_bus_handle_t *bus_handler) {
    i2c_device_config_t rtc_dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x68,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t *master_rtc_handle = malloc(sizeof(i2c_master_dev_handle_t));

    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handler, &rtc_dev_cfg, master_rtc_handle));

    return master_rtc_handle;
}

i2c_master_dev_handle_t* initAndSetupDisplay(i2c_master_bus_handle_t *bus_handler) {
    i2c_device_config_t rtc_dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x68,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t *master_rtc_handle = malloc(sizeof(i2c_master_dev_handle_t));

    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handler, &rtc_dev_cfg, master_rtc_handle));

    return master_rtc_handle;
}

void init_and_setup_ssd1306(
    SSD1306_t *display_device
) {
    ssd1306_init(display_device, 128, 64);

	vTaskDelay(pdMS_TO_TICKS(500));

	ssd1306_clear_screen(display_device, false);
	ssd1306_contrast(display_device, 0xff);
}

void renderDisplayIterationHandler(
    i2c_master_dev_handle_t master_rtc_handle,
    SSD1306_t *display_device
) {
    static char timeFormat[] = "Time: %02d:%02d:%02d";
    static char dateFormat[] = "%s, %02d/%02d/%d";
    static char timeString[15];
    static char dateString[15];

    RtcResponse rtcResponse;

    read_rtc(master_rtc_handle, &rtcResponse);
    
    snprintf(timeString, strlen(timeFormat), timeFormat, rtcResponse.Hour, rtcResponse.Minute, rtcResponse.Second);
    snprintf(dateString, strlen(dateFormat) + 3, dateFormat, DAYS[rtcResponse.DayOfWeek], rtcResponse.Day, rtcResponse.Month, rtcResponse.Year);

    ssd1306_display_text(display_device, 1, timeString, sizeof(timeString), 0); 
    ssd1306_display_text(display_device, 3, dateString, sizeof(timeString), 0); 
}

void app_main() {
    i2c_master_dev_handle_t *master_rtc_handle;
    SSD1306_t display_device;

	i2c_master_init(&display_device, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_RST_PIN);
    
    master_rtc_handle = connectRTC(&display_device._i2c_bus_handle);

    init_and_setup_ssd1306(&display_device);

    while (1) {
        renderDisplayIterationHandler(*master_rtc_handle, &display_device);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

