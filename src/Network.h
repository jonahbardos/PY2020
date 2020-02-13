#pragma once

#include <cstdint>

#include "Constants.h"
#include "Globals.h"

enum class PacketKind
{
	CAN,
	Network,
};

struct Packet
{
	PacketKind kind;
	int address;
	uint8_t payload[Constants::PACKET_PAYLOAD_SIZE];
};

void InitializeNetwork();
void ParseIncomingNetworkPackets();
void SendOutgoingNetworkPackets();
