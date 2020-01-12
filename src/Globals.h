#pragma once

#include "CommandLineOptions.h"
#include "Network.h"
#include "StatusData.h"

#include <vector>

enum class RoverState
{
    RemoteControl,
    DrivingToWaypoint,
    LookingForTennisball,
};

// NOTE(sasha): To keep linker happy, declare globals with "extern" here and then
//              provide variable definition in Globals.cpp
namespace Globals
{
    extern CommandLineOptions opts;
    extern RoverState curr_state;
    extern std::vector<Packet> incoming_packets;
    extern std::vector<Packet> outgoing_packets;
    extern int can_fd;
    extern int net_fd;
    extern StatusData status_data;
}
