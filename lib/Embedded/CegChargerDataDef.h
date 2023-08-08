#ifndef CEGCHARGER_DATA_H
#define CEGCHARGER_DATA_H

#include <Vector.h>

namespace CEG_CHARGER {
    enum ErrorType {
        No_Error = 0x0,
        Invalid_Command = 0x02,
        Data_Invalid = 0x03,
        In_Start_Processing = 0x07
    };
    enum DeviceNumber {
        Single_Module = 0x0A,
        Group_Module = 0x0B,
    };
    enum CommandNumber {
        Read_System_Output_Voltage_Current_Information = 0x01,
        Read_System_Number_Information = 0x02,
        Read_Module_Output_Voltage_Current_Information = 0x03,
        Read_Module_Number_Temperature_State_Information = 0x04,
        Read_Module_AC_Input_information = 0x06,
        Read_Module_Version_Information = 0x07,
        Read_Module_Rated_Output = 0x0a,
        Read_Module_Barcode_Information = 0x0b,
        Read_Module_External_Voltage_Available_Current = 0x0c,
        Set_Walk_in = 0x13,
        Set_Blink = 0x14,
        Set_On_Off = 0x1a,
        Set_System_Output_Voltage_Current = 0x1b,
        Set_Module_Output_Voltage_Current = 0x1c
    };
}

namespace MessageType {
    enum MessageType {
        Query_Controller = 0x50,
        Query_Module_Response = 0x41,
        Single_Setting_Controller = 0x03,
        Single_Setting_Module_Response = 0x43
    };
}

namespace MessageIdRequest {
    enum MessageIdRequest {
        Module_On_Off_32 = 0x0230,
        Module_On_Off_64 = 0x0232,
        Module_Online_Status_32 = 0xfffe,
        Module_Online_Status_64 = 0xffff,
        Module_Voltage_Mode = 0x0233,
        Module_Output_Voltage = 0x0021,
        Module_Output_Current = 0x0022,
        Module_Modify_Group = 0x001a,
        Query_Single_Module_Info = 0x0202,
    };
}

namespace MessageIdResponse {
    enum MessageIdResponse {
        Module_Operating_Status = 0x0202,
        Module_DC_Status = 0x0203,
        Module_AC_Status = 0x0204,
        Module_Output_Voltage = 0x0205,
        Module_Output_Current = 0x0206,
        Module_Version = 0x020a,
        Module_Online_Status_32 = 0xfffe,
        Module_Online_Status_64 = 0xffff,
    };
}

struct CegData {
    struct SystemData 
    {
        float systemVoltage;
        float totalSystemCurrent;
        uint8_t connectedModule;
    } systemData;

    struct GroupData 
    {
        float groupVoltage;
        float totalGroupCurrent;
        uint8_t connectedModule;
    } groupData;

    struct ModuleData
    {
        float moduleVoltage;
        float moduleCurrent;
        uint8_t connectedGroup;
        struct ModuleState 
        {
            union State_0
            {
                struct Bits
                {
                    uint8_t outputShort :1;
                    uint8_t :1;
                    uint8_t innerCommInterrupt :1;
                    uint8_t pfcSideAbnormal :1;
                    uint8_t :1;
                    uint8_t dischargeAbnormal :1;
                    uint8_t :2;
                } bitInfo;
                uint8_t val;
            } state0;

            union State_1
            {
                struct Bits
                {
                    uint8_t mdlOffState :1;
                    uint8_t mdlFault :1;
                    uint8_t mdlProtect :1;
                    uint8_t fanFault :1;
                    uint8_t overTemperature :1;
                    uint8_t outputOVerVoltage :1;
                    uint8_t walkInEnable :1;
                    uint8_t commInterrupt :1;
                } bitInfo;
                uint8_t val;
            } state1;

            union State_2
            {
                struct Bits
                {
                    uint8_t powerLimit :1;
                    uint8_t mdlIdRepetition :1;
                    uint8_t loadUnsharing :1;
                    uint8_t inputPhaseLost :1;
                    uint8_t inputUnbalance :1;
                    uint8_t inputUnderVoltage :1;
                    uint8_t inputOverVoltage :1;
                    uint8_t pfcSideOff :1;
                } bitInfo;
                uint8_t val;
            } state2;
            
        } moduleState;
    } moduleData[12];

};

struct RequestCommand {
    int msgId;
    int32_t value;
    uint8_t groupNumber;
    uint8_t subAddress;
};

struct ApiRequestCommand {
    int groupNumber = -1;
    int subAddress = -1;
    int32_t value = -1;
};

struct DCOperatingStatus {
    int chargerInitStatus;
    int pfcSoftStartInit_1;
    int pfcSoftStartInit_2;
    int pfcSoftStart;
    int dcdcSoftStart;
    int dcdcSoftStartWideningStage;
    int dcdcSoftStartFmStage;
    int moduleNormal;
    int moduleShutdown;
};

struct ACOperatingStatus {
    int init;
    int standby;
    int acdcSoftStart;
    int normal;
    int alarm;
    int monitorShutdown;
};

struct DCStatus_1 {
    bool outputOvervoltage;
    bool overTemperature;
    bool hardwareFailure;
    bool operatingMode;
    bool fanFailure;
    bool acLimitedState;
    bool temperatureDerating;
    bool powerLimitedState;
    bool moduleOn;
    bool ouputUndervoltage;
    bool moduleAddressConflict;
    bool currentImbalance;
    bool canCommFail;
};

struct DCStatus_2 {
    bool shortCircuit;
    bool currentImbalance_20s;
    bool alarmCapacity;
    bool failureCapacity;
};

struct ACStatus_1 {
    bool inputCurrentPhaseLoss;
    bool inputPhaseLoss_A;
    bool inputPhaseLoss_B;
    bool inputPhaseLoss_C;
    bool busOvervoltage;
    bool busUndervoltage;
    bool busVoltageImbalance;
    bool busVoltageImbalanceOverLimit;
    bool inputOverload;
    bool inputOverloadOverlimit;
    bool inputOvercurrent;
};

struct ACStatus_2 {
    bool inputOvervoltage;
    bool inputUndervoltage;
    bool inputOverfrequency;
    bool inputUnderfrequency;
    bool voltageImbalance;
    bool sidePhaseLockFailure;
    bool voltageFastPowerDown;
    bool abnormalInput;
    bool busOvervoltageManyTimes;
    bool wrongSoftwareVer;
    bool sciCommError;
};

struct DataCharger {
    uint16_t msgCount;
    int monitorGroup = -1;
    int monitorSubAddress = -1;
    int groupNumber = -1;
    int subAddress = -1;
    int moduleOff = 1;
    int dcOperatingStatus;
    int acOperatingStatus;
    int dcStatus_1;
    int dcStatus_2;
    int acStatus_1;
    int acStatus_2;
    int32_t outputVoltage;
    int32_t outputCurrent;
    int acVersionNumber;
    int dcVersionNumber;
    // DCOperatingStatus dcOperatingStatus;
    // ACOperatingStatus acOperatingStatus;
    // DCStatus_1 dcStatus_1;
    // DCStatus_2 dcStatus_2;
    // ACStatus_1 acStatus_1;
    // ACStatus_2 acStatus_2;
};

union FrameId
{
    struct FrameField {
        uint32_t sourceAddress : 8;
        uint32_t destinationAddress : 8;
        uint32_t commandNumber : 6;
        uint32_t deviceNumber : 4;
        uint32_t errorCode : 3;
    } frameField;
    uint32_t id;
};

struct CanMessage {
    FrameId frameId = {
        .id = 0x0
    };
    bool extended;
    bool rtr;
    uint8_t dlc;
    uint8_t data[8];
};
    

#endif