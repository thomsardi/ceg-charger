#ifndef CAN_BASE_CLASS_H
#define CAN_BASE_CLASS_H

#include <Arduino.h>
#include <CAN.h>

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

class CANBaseClass : public ESP32SJA1000Class{
    public :
        CANBaseClass();
        int filterExtended(long id, long mask);
        int endPacket();
        int loopback();

    private :
        void modifyRegister(uint8_t address, uint8_t mask, uint8_t value);
        void writeRegister(uint8_t address, uint8_t value);
        int xnor(int a, int b);
        void swap(int &a, int &b);
        uint8_t readRegister(uint8_t address);
        bool _loopback;        
};


#endif