#include "../Globals.h"
#include "ParseCAN.h"
#include "ParseBaseStation.h"
#include "TestPackets.h"
#include "NetworkingStubs.h"
#include "IK.h"
extern "C"
{
    #include "../HindsightCAN/Port.h"
    #include "../HindsightCAN/CANMotorUnit.h"
}
#include <catch2/catch.hpp>
#include <iostream>

using nlohmann::json;

TEST_CASE("Basic CAN ID construction", "[CAN]")
{
    uint16_t CAN_ID = ConstructCANID(PACKET_PRIORITY_NORMAL, DEVICE_GROUP_MOTOR_CONTROL, 0x05);
    uint16_t EXPECTED = 0x0505;
    REQUIRE(CAN_ID == EXPECTED);
}

TEST_CASE("Basic packet creation", "[CAN]")
{
    uint16_t CAN_ID = ConstructCANID(PACKET_PRIORITY_NORMAL, DEVICE_GROUP_MOTOR_CONTROL, 0x05);
    uint8_t testDataPacket[2];
    testDataPacket[0] = 0x60;
    testDataPacket[1] = 0x43;
    CANPacket packet = ConstructCANPacket(CAN_ID, 0x02, testDataPacket);

    REQUIRE(packet.id == 0x0505);
    REQUIRE(packet.dlc == 0x02);
    REQUIRE(packet.data[0] == 0x60);
    REQUIRE(packet.data[1] == 0x43);
}

TEST_CASE("ParseCAN can handle telemetry", "[CAN]")
{
    ParseCANPacket(motorTelemetry());
    std::cout << Globals::status_data << std::endl;
    REQUIRE(Globals::status_data.dump() == "{\"front_right_wheel\":{\"current\":256}}");
}

TEST_CASE("Can change motor mode", "[BaseStation]")
{
    clearPackets();
    char const *msg = "{\"type\": \"motor\", \"motor\": \"front_right_wheel\", \"mode\": \"PID\"}";
    REQUIRE(ParseBaseStationPacket(msg) == true);
    REQUIRE(popBaseStationPacket() == "{\"status\":\"ok\"}");
    REQUIRE(Globals::motor_status["front_right_wheel"]["mode"] == "PID");
    CANPacket p = popCANPacket();
    REQUIRE(PacketIsOfID(&p, ID_MOTOR_UNIT_MODE_SEL));
    REQUIRE(p.dlc == 2);
    REQUIRE(p.data[1] == MOTOR_UNIT_MODE_PID);
    REQUIRE(GetDeviceGroupCode(&p) == DEVICE_GROUP_MOTOR_CONTROL);
    REQUIRE(GetDeviceSerialNumber(&p) == 0x09); // front right wheel
}

TEST_CASE("Can set P coefficient", "[BaseStation]")
{
    clearPackets();
    char const *msg = "{\"type\": \"motor\", \"motor\": \"front_right_wheel\", \"P\": 256}";
    REQUIRE(ParseBaseStationPacket(msg) == true);
    CANPacket p = popCANPacket();
    REQUIRE(PacketIsOfID(&p, ID_MOTOR_UNIT_PID_P_SET));
    REQUIRE(p.dlc == 5);
    REQUIRE(p.data[1] == 0);
    REQUIRE(p.data[2] == 0);
    REQUIRE(p.data[3] == 1); // 256 (MSB first)
    REQUIRE(p.data[4] == 0);
}

TEST_CASE("Does not send CAN packets if nothing changes", "[BaseStation]")
{
    clearPackets();
    REQUIRE(numCANPackets() == 0);
    // This motor name needs to be different from the previous one, because the motor_status object is
    // preserved across test cases.
    char const *msg = "{\"type\": \"motor\", \"motor\": \"arm_base\", \"mode\": \"PID\"}";
    REQUIRE(ParseBaseStationPacket(msg) == true);
    REQUIRE(numCANPackets() == 1);
    popCANPacket();
    REQUIRE(numCANPackets() == 0);
    REQUIRE(ParseBaseStationPacket(msg) == true);
    REQUIRE(numCANPackets() == 0);
}

TEST_CASE("Can handle malformed packets", "[BaseStation]")
{
    char const *msg = "{\"type\": \"moto";
    REQUIRE(ParseBaseStationPacket(msg) == false);
}

TEST_CASE("Ignores invalid motor names", "[BaseStation]")
{
    char const *msg = "{\"type\": \"motor\", \"motor\": \"xyzzy\", \"mode\": \"PID\"}";
    REQUIRE(ParseBaseStationPacket(msg) == false);
}

TEST_CASE("Can handle drive packets", "[BaseStation]")
{
    char const *msg = "{\"type\":\"drive\",\"forward_backward\":0.3,\"left_right\":-0.4}";
    REQUIRE(ParseBaseStationPacket(msg) == true);
    REQUIRE(Globals::motor_status["front_right_wheel"]["PWM target"] == -50);
}

TEST_CASE("Can handle malformed drive packets", "[BaseStation]")
{
    char const *msg = "{\"type\":\"drive\",\"forward_backward\":2.9}";
    REQUIRE(ParseBaseStationPacket(msg) == false);
}

void assert_IK_equals(double base, double shoulder, double elbow)
{
    CANPacket p;
    p = popCANPacket();
    int base_val = DecodeBytesToIntMSBFirst(p.data, 1, 5);
    REQUIRE(intToRad(base_val, 0, 0) == Approx(base));
    p = popCANPacket();
    int shoulder_val = DecodeBytesToIntMSBFirst(p.data, 1, 5);
    REQUIRE(intToRad(shoulder_val, 0, 0) == Approx(shoulder));
    p = popCANPacket();
    int elbow_val = DecodeBytesToIntMSBFirst(p.data, 1, 5);
    REQUIRE(intToRad(elbow_val, 0, 0) == Approx(elbow));
}

TEST_CASE("Can handle IK packets", "[BaseStation]")
{
    char const *msg = "{\"type\":\"ik\",\"wrist_base_target\":[1.3, 0.0, 0.0]}";
    REQUIRE(ParseBaseStationPacket(msg) == true);
    assert_IK_equals(0., 0., 0.);
}
