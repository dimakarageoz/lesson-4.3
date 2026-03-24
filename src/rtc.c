#include <stdio.h>
#include <driver/i2c_master.h>
#include <driver/i2c_types.h>
#include <esp_err.h>

#include "rtc.h"

static int rtc_buffer_size = 7;

uint8_t bcd_to_decimal(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t decimal_to_bcd(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

void read_rtc(
    i2c_master_dev_handle_t rtc_handle,
    RtcResponse *response
) {
    uint8_t buffer[rtc_buffer_size];
    uint8_t bufferBCD[rtc_buffer_size];
    uint8_t startAddress = 0x00;

    ESP_ERROR_CHECK(i2c_master_transmit_receive(rtc_handle, &startAddress, 1, buffer, rtc_buffer_size, -1));

    uint8_t bcdMask;
    uint8_t bcdMask2 = 0xFF;

    for (int i = 0; i < rtc_buffer_size; i++) {
        bcdMask = 0b11111111;

        if (i == 0) {
            bcdMask &= ~(1 << 7); // we need cut last bit because it contains internal command r/w
        }

        if (i == 2) {
            bcdMask &= ~((1 << 7) | (1 << 6));  // exclude bits that contains AM/PM formats
        }

        bufferBCD[i] = bcd_to_decimal(*(buffer + i) & bcdMask);
    }

    response->Second = bufferBCD[0];
    response->Minute = bufferBCD[1];
    response->Hour = bufferBCD[2];
    response->DayOfWeek = bufferBCD[3];
    response->Day = bufferBCD[4];
    response->Month = bufferBCD[5];    
    response->Year = bufferBCD[6];
}