#include "CommandLineOptions.h"
#include "Globals.h"
#include "Networking/Network.h"
#include "Networking/CANUtils.h"
#include "Networking/ParseCAN.h"

void InitializeRover()
{
    InitializeCANSocket();
    InitializeBaseStationSocket();
}

int main(int argc, char **argv)
{
    Globals::opts = ParseCommandLineOptions(argc, argv);
    InitializeRover();
    for(;;)
    {
        // These methods also send packets if necessary, depending
        // on what they receive. (Not implemented yet.)
        CANPacket packet = recvCANPacket();
        ParseCANPacket(packet);
        recvBaseStationPacket();
    }
    return 0;
}
