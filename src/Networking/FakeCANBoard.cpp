
#include "CANUtils.h"
#include "TestPackets.h"
#include <iostream>

int main() {
  InitializeCANSocket();

  std::string str;
  std::cout << "Press enter to send a CAN packet > ";
  while(1) {
    std::getline(std::cin, str);
    sendCANPacket(motorTelemetry());
  }
}
