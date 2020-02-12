#include "Network.h"
#include "Globals.h"

#include <cassert>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

void InitializeNetwork()
{
#ifdef __linux__
	if (Globals::can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW))
		assert(!"Failed to initialize CAN bus!");
#endif
}

// TODO(sasha): We probably want all of this to use asynchronous IO
//              Check out POSIX aio.
void ParseIncomingNetworkPackets()
{
}

void SendOutgoingNetworkPackets()
{
	// NOTE(sasha): This is sort of pseudocode for what we want to do here.
#if 0
    for(const Packet &p : Globals::outgoing_packets)
    {
        if(p.kind == PacketKind::CAN)
            aio_write(Globals::can_fd, p);
        else if(p.kind = PacketKind::Network)
            aio_write(Globals::net_fd, p);
    }
#endif
}
