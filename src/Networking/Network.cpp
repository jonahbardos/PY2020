#include "../Globals.h"
#include "Network.h"
#include "ParseCAN.h"
#include "NetworkConstants.h"

#include <cassert>

#include <net/if.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include<iostream>

int base_station_fd = -1;
bool connected = false;

bool InitializeBaseStationSocket()
{
  if (connected) return connected;

  if ((base_station_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Base station socket creation failed");
    return false;
  }

  struct sockaddr_in servaddr;
  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

  if (connect(base_station_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("Base station connect failed");
    return false;
  }

  return (connected = true);
}

int recvBaseStationPacket(char *buffer)
{
  if (!connected) return 0;

  // TODO: How do we split base station packets? If we split on newlines
  // this will require more complicated buffer management.
  // If we split by requiring the base station to send a four-byte length
  // before sending each packet, we might need to dynamically allocate
  // memory. Either way we may need to handle blocking reads more carefully.
  int ret = recv(base_station_fd, buffer, MAXLINE, MSG_DONTWAIT);
  if (ret < 0)
  {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("Failed to read from base station");
    }
    return 0;
  } else if (ret == 0) {
    perror("Base station disconnected");
    exit(1);
  }
  return 1;
}

void sendBaseStationPacket(const std::string &packet) {
  if (write(base_station_fd, packet.c_str(), packet.length()) < 0)
  {
    perror("Failed to send to base station");
  }
}
