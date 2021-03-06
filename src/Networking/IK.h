
#pragma once

#include "json.hpp"
using nlohmann::json;
bool ParseIKPacket(json &message);

// Convert angle in radians to the discrete number used as a PID target
//   offset: the radian angle corresponding to int angle 0 (depends on the joint)
//   flip: +/- 1, whether the int sign is opposite the radian sign (depends on the joint)
// Radian angle zero is chosen so that the typical "stowed" arm angles
// are approximately (0,pi,pi).
// Arm base positive direction is counterclockwise (viewed from above the rover).
// Shoulder, elbow positive direction is towards the stowed limit switch.
int32_t radToInt(double d, double offset, int32_t sign_flip);

// Inverse of the above
double intToRad(int32_t i, double offset, int32_t sign_flip);
