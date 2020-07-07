
#include "Network.h"
#include "../Globals.h"
#include "CANUtils.h"
#include "ParseCAN.h"
#include "log.h"
#include "IK.h"
#include "ParseBaseStation.h"
#include <cmath>
#include <tgmath.h>

extern "C"
{
    #include "../HindsightCAN/CANMotorUnit.h"
}

#include <iostream>

using nlohmann::json;

// It's important that all of these vectors are sorted in the same order (the indices correspond)
std::map<std::string, std::vector<std::string>> possible_keys = {
	{"motor", {"mode", "P", "I", "D", "PID target", "PWM target"}},
};
std::vector<int> motor_packet_ids = {
  ID_MOTOR_UNIT_MODE_SEL,
  ID_MOTOR_UNIT_PID_P_SET,
  ID_MOTOR_UNIT_PID_I_SET,
  ID_MOTOR_UNIT_PID_D_SET,
  ID_MOTOR_UNIT_PID_POS_TGT_SET,
  ID_MOTOR_UNIT_PWM_DIR_SET
};
std::vector<int> motor_packet_lengths = {
  2, 5, 5, 5, 5, 5
};

bool SendCANPacketToMotor(json &message, int key_idx, uint16_t CAN_ID);
bool ParseDrivePacket(json &message);
bool ParsePuppetPacket(json &message);
bool sendCANPacketAngle(std::string joint, int angle);

int getIndex(const std::vector<std::string> &arr, std::string &value)
{
  for (int i = 0; i < arr.size(); i++) {
    if (arr[i] == value) {
      return i;
    }
  }
  return -1;
}

bool sendError(std::string const &msg)
{
  json error_message = {};
  error_message["status"] = "error";
  error_message["msg"] = msg;
  sendBaseStationPacket(error_message.dump());
  log(LOG_ERROR, error_message.dump());
  return false;
}

bool ParseBaseStationPacket(char const* buffer)
{
  log(LOG_INFO, "Message from base station: " + (std::string) buffer);
  json parsed_message;
  try
  {
    parsed_message = json::parse(buffer);
  }
  catch (json::parse_error)
  {
    return sendError("Parse error");
  }
  std::string type;
  try
  {
    type = parsed_message["type"];
  }
  catch (json::type_error)
  {
    return sendError("Could not find message type");
  }
  log(LOG_DEBUG, "Message type: " + type);
  bool success;
  if (type == "ik") {
    success = ParseIKPacket(parsed_message);
  }
  else if (type == "drive") {
    success = ParseDrivePacket(parsed_message);
  }
  else if (type == "motor") {
    success = ParseMotorPacket(parsed_message);
  }
  else if (type == "puppet") {
    success = ParsePuppetPacket(parsed_message);
  }
  if (success)
  {
    json response = {{"status", "ok"}};
    sendBaseStationPacket(response.dump());
  }
  return success;
}

bool ParseMotorPacket(json &message)
{
  std::string motor;
  try
  {
    motor = message["motor"];
  }
  catch (json::type_error)
  {
    return sendError("No motor specified");
  }
  int motor_serial = getIndex(motor_group, motor);
  if (motor_serial < 0)
  {
    return sendError("Unrecognized motor " + motor);
  }
  log(LOG_DEBUG, "Parsing motor packet for motor " + motor);
  Globals::motor_status[motor]["motor"] = motor;
  log(LOG_DEBUG, "Original status: " + Globals::motor_status[motor].dump());


  for (int key_idx = 0; key_idx < possible_keys["motor"].size(); key_idx++) {
    std::string key = possible_keys["motor"][key_idx];
    if (message[key] != nullptr && message[key] != Globals::motor_status[motor][key]) {
      log(LOG_DEBUG, "Updating " + key);
      uint16_t CAN_ID = ConstructCANID(PACKET_PRIORITY_NORMAL,
                                       DEVICE_GROUP_MOTOR_CONTROL,
                                       motor_serial);
      if (SendCANPacketToMotor(message, key_idx, CAN_ID)) {
        Globals::motor_status[motor][key] = message[key];
      } else {
        return sendError("Failed to send CAN packet to motor " + motor);
      }
    }
  }

  return true;
}

bool ParseDrivePacket(json &message)
{
  double fb, lr;
  try
  {
    fb = message["forward_backward"];
    lr = message["left_right"];
  }
  catch (json::type_error)
  {
    return sendError("Malformatted drive packet");
  }
  if (fb > 1.0 || fb < -1.0 || lr > 1.0 || lr < -1.0)
  {
    return sendError("Drive targets not within bounds +/- 1.0");
  }
  // TODO I'm curious how intuitive it would be to use remote control with wheel velocities rather
  // than kinematic velocities. That would actually allow the rover to go faster, I think.
  // Another alternative: add a boolean "wheel_velocities" indicating which mode to use
  // (if true, the other keys should just be "right" and "left").
  double right = (fb + lr)/2;
  double left =  (fb - lr)/2;
  int max_pwm = 1000; // TODO figure out what is the maximum value the hardware supports
  int right_pwm = (int) max_pwm * right;
  int left_pwm = (int) max_pwm * left;
  json packet = {};
  packet["type"] = "motor";
  packet["mode"] = "PWM";
  // TODO we may need to switch some of these signs
  packet["PWM target"] = right_pwm;
  packet["motor"] = "front_right_wheel";
  ParseMotorPacket(packet);
  packet["motor"] = "back_right_wheel";
  ParseMotorPacket(packet);
  packet["PWM target"] = left_pwm;
  packet["motor"] = "front_left_wheel";
  ParseMotorPacket(packet);
  packet["motor"] = "back_left_wheel";
  ParseMotorPacket(packet);
  return true;
}

bool ParsePuppetPacket(json &message)
{
  // Propably won't be getting values for all of these angles
  uint16_t base, shoulder, elbow, forearm, diff_l, diff_r, hand;
  try
  {
    base = message["base"];
    shoulder = message["shoulder"];
    elbow = message["elbow"];
    forearm = message["forearm"];
    diff_l = message["diff_l"];
    diff_r = message["diff_r"];
    hand = message["hand"];
  }
  catch (json::type_error)
  {
    return sendError("Malformatted puppet packet");
  }
  if (!sendCANPacketAngle("arm_base", base)) { return false; }
  if (!sendCANPacketAngle("shoulder", shoulder)) { return false; }
  if (!sendCANPacketAngle("elbow", elbow)) { return false; }
  if (!sendCANPacketAngle("forearm", forearm)) { return false; }
  if (!sendCANPacketAngle("diffleft", diff_l)) { return false; }
  if (!sendCANPacketAngle("diffright", diff_r)) { return false; }
  if (!sendCANPacketAngle("hand", hand)) { return false; }

  return true;
}

bool sendCANPacketAngle(std::string joint, int angle)
{
  // Assumes negative angles mean value is not used in puppet control
  if (angle < 0) 
  {
    return true;
  }
  
  json base_packet = {{"type", "motor"},
                      {"motor", joint},
                      {"mode", "PID"},
                      {"PID target", angle}}; 
  if (!ParseMotorPacket(base_packet))
  {
    return false;
  }
  return true;
}

bool SendCANPacketToMotor(json &message, int key_idx, uint16_t CAN_ID) {
  std::string key = possible_keys["motor"][key_idx];
  int length = motor_packet_lengths[key_idx];
  int packet_id = motor_packet_ids[key_idx];
  uint8_t data[length];
  WritePacketIDOnly(data, packet_id);

  if (key == "mode") {
    std::string mode = message[key];
    if (mode == "PWM") {
      data[1] = MOTOR_UNIT_MODE_PWM;
    } else if (mode == "PID") {
      data[1] = MOTOR_UNIT_MODE_PID;
    } else {
      return sendError("Unrecognized mode " + mode);
    }
  }
  else // key is one of P, I, D, PID target, or PWM target
  {
    uint32_t val = message[key];
    PackIntIntoDataMSBFirst(data, val, 1);
  }

  CANPacket p = ConstructCANPacket(CAN_ID, length, data);
  sendCANPacket(p);
  return true;
}
