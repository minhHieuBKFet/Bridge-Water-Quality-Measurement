/**
 * @file LoraSX1278.h
 * @author Vu Thanh Trung & Nguyen Minh Hieu
 * @brief GPS module 
 * @version 0.1
 * @date 2024-02-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LORASX1278_H
#define LORASX1278_H

#define LORA_SX1278_FREQ 433E6
#define MEASUREMENT_DEVICE_ADDR 0xAA
#define BRIDGE_DEVICE_ADDR 0xBB
// These values are used to avoid multiple clicks on local web 
#define REQUEST_BYTE_1 0x00
#define REQUEST_BYTE_2 0xFF

#define ERROR_LORA_SX1278_INIT_FAILED 0X01
#define ERROR_LORA_SX1278_RECEIVE_FAILED 0X02
#define ERROR_LORA_SX1278_DATA_LENGTH_MISMATCH 0x03
#define ERROR_LORA_SX1278_ADDR_MISMATCH 0x04
#define ERROR_LORA_SX1278_REQUEST_MISMATCH 0x05
#define ERROR_LORA_SX1278_RECEIVE_TIMEOUT 0x06

// Packet structure sent from measurement device: DEST_ADDR | SRC_ADDR | DATA_LEN | DATA
// Packet structure sent from bridge device: DEST_ADDR | SRC_ADDR | REQUEST_BYTE_1 | REQUEST_BYTE_2 | DENSITY_WATER

#include <LoRa.h>
#include "configs.h"
#include "Datastructure.h"


//extern bool RF_requestData = false;
/**
 * @brief Initialize LoraSX1278 module
 * @return ERROR_NONE: if init successfully
 *         ERROR_LORA_SX1278_INIT_FAILED: if init failed
*/
ERROR_CODE LoraSX1278_Init();

/**
 * @brief Send data from measurement device to bridge device
 * @param data: datastring
 * @param dest_addr: destination address
 * @param src_addr: source address
 * 
*/
void LoraSX1278_sendDataString(char *data, uint8_t dest_addr, uint8_t src_addr);

/**
 * @brief Request data from bridge device
 * @param dest_addr: destination address
 * @param src_addr: source address
 * @param request_byte_1: request byte 1
 * @param request_byte_2: request byte 2
 * @param _densityWater: density of Water (=1 with pure water)
*/
void LoraSX1278_requestData(uint8_t dest_addr, uint8_t src_addr, uint8_t request_byte_1, uint8_t request_byte_2, float _densityWater);

/**
 * @brief Receive data from measurement device
 * @param packetSize: packet size
*/
ERROR_CODE LoraSX1278_receiveData();

/**
 * @brief Receive request from bridge device
 * 
*/
void LoraSX1278_receiveRequest(int packetSize);
//ERROR_CODE LoraSX1278_receiveRequest();

#endif