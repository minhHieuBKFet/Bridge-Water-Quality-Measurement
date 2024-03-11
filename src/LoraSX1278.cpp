#include "LoraSX1278.h"
char* dataSplited[6]; // poiter array to store data splited
extern String dateTime;
extern String latitude;
extern String longitude;
extern String depth;
extern String temp;
extern String DO_value;
extern String nameFileSaveData;
ERROR_CODE LoraSX1278_Init(){
    //DIO0 is optional
    LoRa.setPins(PIN_CS_RF_MODULE, PIN_RST_RF_MODULE, PIN_DIO0_RF_MODULE);
    uint8_t initCount=0;
    Serial.println("Initializing LoRaSX1278...");
    while(!LoRa.begin(LORA_SX1278_FREQ) && initCount<10){
        Serial.print(".");
        delay(1000);
        initCount++;
    }
    if(initCount==10){
        log_e("LoRaSX1278 init failed!");
        return ERROR_LORA_SX1278_INIT_FAILED;
    }
    #ifdef MEASUREMENT_DEVICE
    //Measurement device need to be in receive mode to wait for request
    LoRa.onReceive(LoraSX1278_receiveRequest);
    LoRa.receive();
    #endif
    
    #ifdef BRIDGE_DEVICE
    //set callback function to be called when packet is received
    // LoRa.onReceive(LoraSX1278_receiveData);
    #endif

    Serial.println("LoRaSXX1278 init successfully!");
    return ERROR_NONE;
    
}
void LoraSX1278_sendDataString(char *data, uint8_t dest_addr, uint8_t src_addr){
    LoRa.beginPacket();                   // start packet
    LoRa.write(dest_addr);              // add destination address
    LoRa.write(src_addr);             // add sender address
    LoRa.write(strlen(data));        // add payload length
    LoRa.print(data);                 // add payload
    LoRa.endPacket();                     // finish packet and send it
    log_i("Send packet completely!");
}

void LoraSX1278_requestData(uint8_t dest_addr, uint8_t src_addr, uint8_t request_byte_1, uint8_t request_byte_2, float _densityWater){
    LoRa.beginPacket();                   // start packet
    LoRa.write(dest_addr);              // add destination address
    LoRa.write(src_addr);             // add sender address
    LoRa.write(request_byte_1);        // add REQUEST_BYTE_1
    LoRa.write(request_byte_2);        // add REQUEST_BYTE_2
    LoRa.print(_densityWater);
    LoRa.endPacket();                     // finish packet and send it
    Serial.println("Send request completely!");
}

ERROR_CODE LoraSX1278_receiveData(){
    int packetSize = LoRa.parsePacket();
    long long startReceive = millis();
   // if (packetSize == 0) return;          // if there's no packet, return
    while(!packetSize){
        if(millis()-startReceive >= 30000){
            addDataToList("NULL");
            return ERROR_LORA_SX1278_RECEIVE_TIMEOUT;
        }
        packetSize = LoRa.parsePacket();
    }
    // read packet header bytes:
    uint8_t recipient = LoRa.read();          // recipient address
    uint8_t sender = LoRa.read();            // sender address
    uint8_t incomingDataLength = LoRa.read();    // incoming msg length

    String incomingData = "";                 // payload of packet

    while (LoRa.available()) {            // can't use readString() in callback, so
      incomingData += (char)LoRa.read();      // add bytes one by one
    }

    if (incomingDataLength != incomingData.length()) {   // check length for error
      Serial.println("Datalength does not match length.");
      addDataToList("NULL");
      return ERROR_LORA_SX1278_DATA_LENGTH_MISMATCH;
    }

    // if the recipient isn't this device or broadcast,
    if (recipient != BRIDGE_DEVICE_ADDR && recipient != 0xFF) {
      Serial.println("This message is not for this device.");
      addDataToList("NULL");
      return ERROR_LORA_SX1278_ADDR_MISMATCH;
    }

    //split dataString
    // char* token = strtok((char*)incomingData.c_str(), ",");
    // int count = 0;
    // while(token != NULL){
    //     dataSplited[count] = token;
    //     count++;
    //     token = strtok(NULL, ",");
    // }
    // //sprintf(dataString,"DateTime: %s \tLatitude: %s \tLongitude: %s \tDepth: %s \tTemp: %s \tDO Value: %s", dataSplited[0], dataSplited[1], dataSplited[2], dataSplited[3], dataSplited[4], dataSplited[5]);
    // dateTime = dataSplited[0];
    // latitude = dataSplited[1];
    // longitude = dataSplited[2];
    // depth = dataSplited[3];
    // temp = dataSplited[4];
    // DO_value = dataSplited[5];

    addDataToList(incomingData);
    char* token = strtok((char*)incomingData.c_str(), "T");
    dateTime = token;
    nameFileSaveData = dateTime;
    SPIFFS_saveStringDataToFile(nameFileSaveData, incomingData);

    // if message is for this device, or broadcast, print details:
    // Serial.println("Received from: 0x" + String(sender, HEX));
    // Serial.println("Sent to: 0x" + String(recipient, HEX));
    // Serial.println("Datalength: " + String(incomingDataLength));
    // Serial.println("DateTime: " + String(dataSplited[0]) 
    //                             + "\tLatitue:" + String(dataSplited[1]) 
    //                             + "\tLongitude:" + String(dataSplited[2])
    //                             + "\tDepth:" + String(dataSplited[3])
    //                             + "\tTemp:" + String(dataSplited[4])
    //                             + "\tDO:" + String(dataSplited[5]));
    // Serial.println("RSSI: " + String(LoRa.packetRssi()));
    // Serial.println("Snr: " + String(LoRa.packetSnr()));
    // Serial.println();
    return ERROR_NONE;
}   

void LoraSX1278_receiveRequest(int packetSize ){
    if (packetSize == 0) return;
      // uint8_t recipient = LoRa.read();          // recipient address
      // uint8_t sender = LoRa.read();            // sender address
      // uint8_t request_byte_1 = LoRa.read(); 
      // uint8_t request_byte_2 = LoRa.read(); 
      // if(request_byte_1 == REQUEST_BYTE_1 && request_byte_2 == REQUEST_BYTE_2){
      //   RF_requestData = true;
      //   log_i("Receive request successfully!");
      //   return;
      // }
      // log_e("Receive request failed");
      // return;
    String string = "";
      while(LoRa.available()){
         string = LoRa.readString();
      }
      if(string != ""){
        //RF_requestData = true;
        log_i("Receive request successfully!");
        return;
      } else{
        log_e("Receive request failed");
      }
    log_e("No request received!");
    return;
}