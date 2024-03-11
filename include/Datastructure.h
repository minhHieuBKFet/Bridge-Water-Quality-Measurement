
#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include "configs.h"
#include <Wifi.h>
#include <SPIFFS.h>
#include <WebServer.h>

void addDataToList(String data);

void displayDataList(WiFiClient& client);

void SPIFFS_saveStringDataToFile( String fileName_string, String fileContent_string);

void SPIFFS_checkOutOfMemory();

#endif