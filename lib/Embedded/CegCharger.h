#ifndef CEGCHARGER_H
#define CEGCHARGER_H

#ifndef Arduino_h
    #include <iostream>
#endif
#include <CegChargerDataDef.h>
#include <CAN.h>
#include <CANBaseClass.h>
// #include <ESP32SJA1000.h>
#include <Vector.h>
#include <ArduinoJson.h>

#define REG_BASE                   0x3ff6b000

#define REG_MOD                    0x00
#define REG_CMR                    0x01
#define REG_SR                     0x02
#define REG_IR                     0x03
#define REG_IER                    0x04

#define REG_BTR0                   0x06
#define REG_BTR1                   0x07
#define REG_OCR                    0x08

#define REG_ALC                    0x0b
#define REG_ECC                    0x0c
#define REG_EWLR                   0x0d
#define REG_RXERR                  0x0e
#define REG_TXERR                  0x0f
#define REG_SFF                    0x10
#define REG_EFF                    0x10
#define REG_ACRn(n)                (0x10 + n)
#define REG_AMRn(n)                (0x14 + n)

#define REG_CDR                    0x1F

class CegCharger : public CANBaseClass{
    public :
        CegCharger(int controllerAddress);
        int run();
        int putToQueue(const CanMessage &canMessage);
        int isSendQueueEmpty();
        int getSendQueueSize();
        int getModuleStackSize();

        CegData::ModuleData getModuleData(int index);

        int processPacket(const CanMessage &canMessage);
        
        void printStack();
        void readSystemVoltageCurrent(int deviceNumber, int destinationAddress);
        void readSystemNumberInformation(int deviceNumber, int destinationAddress);
        void readModuleVoltageCurrent(int destinationAddress);
        void readModuleExtraInformation(int destinationAddress);
        void readModuleInputVoltageInformation(int destinationAddress);
        void readModuleExternalVoltageAvailableCurrent(int destinationAddress);
        void setWalkIn(int deviceNumber, int destinationAddress, uint8_t enable = 1, uint16_t time = 0);
        void setBlink(int deviceNumber, int destinationAddress, uint8_t blink = 0);
        void setOnOff(int deviceNumber, int destinationAddress, uint8_t off);
        void setSystemVoltageCurrent(int deviceNumber, int destinationAddress, uint32_t voltage, uint32_t current);
        void setModuleVoltageCurrent(int deviceNumber, int destinationAddress, uint32_t voltage, uint32_t current);

        void cleanUp();

        int parseSyncSystemVoltageCurrentJson(JsonVariant &json);
        int parseSyncGroupVoltageCurrentJson(JsonVariant &json);
        int parseAllGroupVoltageCurrentJson(JsonVariant &json);
        int parseSingleGroupVoltageCurrentJson(JsonVariant &json);
        int parseSetAllModuleJson(JsonVariant &json);
        int parseSetSingleGroupJson(JsonVariant &json);
        int parseSetSingleModuleJson(JsonVariant &json);
        String getDataJson();

        CegData* getStackAddress();

    private :
        CanMessage _canMessageStorage[32];
        Vector<CanMessage> _canMessage;
        void fillStack();
        void insertGroupData(const CegData::GroupData &grpData, int commandNumber);
        void insertModuleData(const CegData::ModuleData &mdlData, int commandNumber);
        int _controllerAddress;
        CegData _cegData;
        Vector<CegData::GroupData> _groupData;
        Vector<CegData::ModuleData> _moduleData;
};

#endif