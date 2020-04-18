
#include "CANUtils.h"
#include <iostream>

extern "C"
{
    #include "../HindsightCAN/CANMotorUnit.h"
}
// TODO: Ports (deviceGroup and deviceSerial) are placeholders in PortTemplate
//       Need to create a port for Jetson and include it

// TODO: Possibly make own header file
void SendSingleCAN();


int main() {
  InitializeCANSocket();

  uint8_t mode;

  bool run = true;
  while(run)
  {
    std::cout << "==================================";
    std::cout << "Choose a Test Mode by Number";
    std::cout << "1 - Receive CAN Packets RAW ";
    std::cout << "2 - Send Single CAN Packets";
    std::cout << "3 - Receive CAN Packets Formated ";
    std::cout << "4 - Send CAN Motor Packets Terminal UI";
    std::cout << "==================================";
    std::cin >> mode;
    
    switch(mode)
    {
      case 1:
        std::cout << "Mode not implemented yet";
        // TODO: Add a mode that just prints out the raw data seen by CAN
        break;
      case 2:
        SendSingleCAN();
        break;
      case 3:
        std::cout << "Mode not implemented yet";
        // TODO: Add a mode that reads CAN data and formats them to be useful
        //       Should have ID's, Device Group, Device Serial labeled based on value
        break;
      case 4:
        std::cout << "Mode not implemented yet";
        // TODO: Add a mode that uses key inputs to control motors
        //       Example of this is using WASD keys in terminal to drive rover
        break;
      default:
        std::cout << "Value not valid. Enter choice again";
        break;
    }

  }
}

void SendSingleCAN()
{
  std::cout << "Entering Send Single CAN Mode";
  bool runSendSingleCAN = true;
  uint8_t deviceGroup;
  while(runSendSingleCAN)
  {
    std::cout << "Enter device group number to send to";
    std::cout << "(0-Broadcast) (1-Reserved) (2-Jetson)";
    std::cout << "(3-Power) (4-Motor) (5-Telemetry) (6-GPIO)";
    std::cin >> deviceGroup;

    // Maybe make this into case statement
    if (deviceGroup == 4)
    {
      std::cout << "Enter device number to send to";
      std::cout << "(1-Base Rotation) (2-Shoulder) (3-Elbow)";
      std::cout << "(4-Forearm) (5-DiffWrist1) (6-DiffWrist2)";
      std::cout << "(7-Hand) (8-ChasisFL) (9-ChasisFR)";
      std::cout << "(10-ChasisBL) (11-ChasisBR) (12-Science)";
      uint8_t deviceSerial;
      std::cin >> deviceSerial;

      // Check if input is valid
      if (deviceSerial < 13 && deviceSerial > 0)
      {
        std::cout << "Enter motor packet ID number";
        std::cout << "(0-Mode Select) (3-PWM Speed)";
        uint8_t motorID;
        std::cin >> motorID;

        CANPacket p;

        if (motorID == 1)
        {
          std::cout << "Enter motor mode (0-PWM) (1-PID)";
          uint8_t motorMode;
          std::cin >> motorMode;
          AssemblePWMDirSetPacket(&p, deviceGroup, deviceSerial, motorMode);
        }
        else if (motorID == 3) 
        {
          std::cout << "Enter motor PWM speed";
          int32_t PWMVal;
          std::cin >> PWMVal;
          AssemblePWMDirSetPacket(&p, deviceGroup, deviceSerial, PWMVal);
        }
        else { std::cout << "Packet ID not supported yet"; }
        // TODO: Add more packet types

      }

    }
    else
    {
      std::cout << "Device group not supported yet";
    }
    // TODO: Add more device groups

  }

}
