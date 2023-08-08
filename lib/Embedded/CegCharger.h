#ifndef CEGCHARGER_H
#define CEGCHARGER_H

#ifndef Arduino_h
    #include <iostream>
#endif
#include <CegChargerDataDef.h>
#include <CAN.h>
#include <Vector.h>

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

class CegCharger : public ESP32SJA1000Class{
    public :
        CegCharger(int controllerAddress);
        int run();
        // int sendRequest(int cmd, int32_t value); //Broadcast command
        // int sendRequest(int cmd, int32_t value, int groupNumber); //Group Broadcast command
        int putToQueue(const CanMessage &canMessage); //Point to Point command
        int32_t getFrameId();
        int endPacket();
        int loopback();
        int getData(int destination[], size_t arrSize);
        int isSendQueueEmpty();
        int getSendQueueSize();
        int processPacket(const CanMessage &canMessage);

        using CANControllerClass::filterExtended;
        virtual int filterExtended(long id, long mask);

    private :
        ESP32SJA1000Class _esp32sja1000class;
        RequestCommand _requestCommand[16];
        Vector<RequestCommand> _commandList;
        CanMessage _canMessageStorage[32];
        Vector<CanMessage> _canMessage;
        int updateFrameId(int msgId);
        int buildFrameId(int protocolCode, int destinationAddr, int sourceAddr);
        int updateData(int msgId, int32_t value);
        int buildData(int32_t value);
        int moduleRequestOnOff_32(int frameId, int buffer[], size_t bufferLength);
        void modifyRegister(uint8_t address, uint8_t mask, uint8_t value);
        void writeRegister(uint8_t address, uint8_t value);
        bool _loopback;
        int _controllerAddress;
        int _destinationAddr;
        int _sourceAddr;
        int _protocolCode;
        uint8_t readRegister(uint8_t address);
        int32_t _frameId = 0;
        int _pCode = 0;
        int _groupNumber = 0;
        int _subAddress = 0;
        int _monitorGroup = 0;
        int _monitorSubAddress = 0;
        uint8_t _msgType = 0;
        uint8_t _errType = 0;
        uint8_t _msgId[2] = {0};
        uint8_t _msgContent[4] = {0};
        uint8_t _data[8] = {0};
        CegData _cegData;
        size_t _groupArrSize = sizeof(_cegData.groupData) / sizeof(_cegData.groupData);
        size_t _moduleArrSize = sizeof(_cegData.groupData[0].moduleData) / sizeof(_cegData.groupData[0].moduleData[0]);
        Vector<CegData::GroupData> _groupData;
        Vector<CegData::GroupData::ModuleData> _moduleData;
};

#endif