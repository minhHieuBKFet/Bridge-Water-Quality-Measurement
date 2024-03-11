#ifndef CONFIGS_H
#define CONFIGS_H

#include <Arduino.h>

#define NAME_DEVICE "SPARC-1"
#define BRIDGE_DEVICE

#define SERIAL_DEBUG_BAUDRATE   (9600)
#define DEVICE_DATA_SAVE_INTERVAL 5000
#define VREF 3300
#define ADC_RES 4096 // ADC Resolution
#define ERROR_CODE int
#define ERROR_NONE (0x00)
#define MAC_ADDRESS "D4:8A:FC:CE:EB:90"
enum status_et
{
    DISCONNECTED,
    CONNECTED,
    CONNECTION_LOST,
    SCANNING,
    SENDING_DATA,
    READING_DATA,
    WRITING_DATA
};

struct connectionStatus
{
    status_et sdCardStatus;
    status_et ds3231Status;
    status_et rfStatus;
    status_et gpsStatus;
};

extern struct connectionStatus connectionStatus_st;
//========================== DEFINE FOR LOGGING ========================

#define USING_SDCARD
#define USING_RTC

//========================== DEFINE FOR POWER CONTROL ========================

#define PIN_NUM_12V_CTRL   (25)
#define PIN_NUM_5V_CTRL   (26)

//========================== DEFINE FOR SPI  ========================

#define PIN_NUM_MISO       (19)
#define PIN_NUM_MOSI       (23)
#define PIN_NUM_CLK        (18)
#define PIN_CS_RF_MODULE   (5)

#define PIN_RST_RF_MODULE  (14)
#define PIN_DIO0_RF_MODULE (2)


#endif // CONFIGS_H