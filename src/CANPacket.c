#include "CANPacket.h"

// Contains functons for creating CAN packets
// Functions here will be used in Jetson (Rover.cpp) and electronics boards

// Constructs a CAN ID according to standards set by electronics subsystem
// for PY2020 rover. Not compatible with Orpheus (PY2019)
// Inputs: 
//      priority:   A byte determing if the packet should be prioritized
//                  High priority would mean setting this value to 0
//                  Low priority would mean setting this value to 1
//      devGroup:   ID of device group
//      devSerial:  Serial value of device in the device group
// Output:
//      CANID:      CAN ID with correct formatting        
uint16_t ConstructCANID(uint8_t priority, uint8_t devGroup, uint8_t devSerial)
{
    uint16_t CANID = 0x0000;
    CANID = CANID | ((priority & 0x01) << 10);
    CANID = CANID | ((devGroup & 0x0F) << 6);
    CANID = CANID | (devSerial & 0x2F);

    return CANID;
}

// Creates a CANPacket that can be used by fuctions in this file
// Inputs:
//      id:         CAN ID for the packet
//      dlc:        Data length for the packet. It's the number of bytes used
//                  in data payload for packet
//      data:       An array of bytes used for sending data over CAN
// Outputs:
//      CANPacket:  A struct used for storing the parts needed for a CAN Packet
CANPacket ConstructCANPacket(uint16_t id, uint8_t dlc, uint8_t* data)
{
    struct CANPacket cp;
    cp.id = id;
    cp.dlc = dlc;   
    for(int i = 0; i < dlc; i++){
        uint8_t datai = *(data + i);
        cp.data[i] = datai;
    }

    return cp;
}

// Gets the sender device group number from the payload data
// Inputs:
//      data:       Address of the byte array of the payload from CAN packet
// Outputs:
//                  A byte representing the sender device number
uint8_t ParseDataSenderDevice(char* data)
{
    uint8_t data0 = *(data);
    uint8_t data1 = *(data + 1);
    return ((data0 & 0xC0) >> 4) + ((data1 & 0xC0) >> 6);
}

// Gets the sender device serial number from the payload data
// Inputs:
//      data:       Address of the byte array of the payload from CAN packet
// Outputs:
//                  A byte representing the sender device number
uint8_t ParseDataSenderSerial(char* data)
{
    uint8_t data1 = *(data + 1);
    return (data1 & 0x2F);
}

// Gets the packet payload type from the payload data
// Inputs:
//      data:       Address of the byte array of the payload from CAN packet
// Outputs:
//                  A byte representing the sender device number
uint8_t ParseDataPayloadType(char* data)
{
    uint8_t data0 = *(data);
    return (data0 & 0x2F);
}