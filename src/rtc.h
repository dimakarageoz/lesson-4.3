#include <stdio.h>
#include <driver/i2c_types.h>

typedef struct RtcResponse {
    uint8_t Day;
    uint8_t Month;
    uint8_t Year;
    uint8_t DayOfWeek;
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
} RtcResponse;

void read_rtc(
    i2c_master_dev_handle_t rtc_handle,
    RtcResponse *response
);