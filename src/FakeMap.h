#pragma once

#include <vector>
#include <iostream>
#include <cmath>
#include "lidar/PointCloudProcessing.h"
#include "WorldData.h"

class Autonomous;

class FakeMap : public WorldData
{
public:
    FakeMap(Autonomous &autonomous_);
    //Checks if there is an obstacle 1 unit in front of the robot
    bool lidarSees() override;
    //Returns GPS of robot
    PointXY getGPS() override;
    //Returns heading of robot
    float getHeading() override;
    //starts the autonomous pathing
    bool callAutonomous(int maxSteps);
    //Returns the distance of the target if distance is <1
    float targetDistance();
    // Adds a line-shaped obstacle to the map with endpoints first and second
    void addObstacle(PointXY first, PointXY second);
    //Get the robot's pos
    PointXY getRobotPos();

private:
    //updates the robot's position and heading
    void update(std::pair<float, float> directions);
    //makes a line in the form of ax + by = c
    std::vector<float> makeLine(float x2, float y2, float x1, float y1);
    Autonomous &autonomous;
    float heading; //robot's current heading
    PointXY robotPos; //robot's current position
    std::vector<std::pair<PointXY, PointXY>> obstacles; //vector of obstacles
    PointXY target; //robot's target point
    int steps; //Counter for number of actions robot has taken
    float margin; //Margin of error
};
