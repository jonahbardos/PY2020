
#ifndef __PARSE_BASE_STATION_H__
#define __PARSE_BASE_STATION_H__

#include "json.hpp"
using nlohmann::json;
bool ParseBaseStationPacket(char const* buffer);
bool sendError(std::string const &msg);

#endif
