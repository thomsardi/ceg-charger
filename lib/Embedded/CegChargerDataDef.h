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

struct CegData 
{
    struct SystemData 
    {
        uint32_t counter = 0;
        float systemVoltage = 0;
        float totalSystemCurrent = 0;
        uint8_t connectedModule = 0;
    } systemData;

    struct GroupData 
    {
        uint32_t counter = 0;
        uint8_t number = 0;
        float groupVoltage = 0;
        float totalGroupCurrent = 0;
        uint8_t connectedModule = 0;
    } grpData[8];

    struct ModuleData
    {
        uint32_t counter = 0;
        uint8_t number = 0;
        float moduleVoltage = 0;
        float moduleCurrent = 0;
        uint8_t connectedGroup = 0;
        int8_t temperature = 0;
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
                uint8_t val = 0;
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
                uint8_t val = 0;
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
                uint8_t val = 0;
            } state2;
            
        } moduleState;
    } mdlData[16];
};

union FloatRepresent{
    float floatVal = 0;
    int32_t integerVal;
    uint8_t buff[4];
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
    uint8_t data[8] = {0};
};
    

#endif