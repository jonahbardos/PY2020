#include "Autonomous.h"
#include "simulator/world_interface.h"
#include <cmath>

constexpr float PI = M_PI;

Autonomous::Autonomous(PointXY _target) : target(_target)
{
    state = 0;
    targetHeading = -1;
    forwardCount = -1;
    rightTurn = false;
}

PointXY point_tToPointXY(point_t pnt){
    return PointXY{static_cast<float>(pnt(0)), static_cast<float>(pnt(1))};
}

const std::vector<PointXY> points_tToPointXYs(points_t pnts) {
  std::vector<PointXY> res;
  for(point_t &pnt : pnts) {
    res.push_back(point_tToPointXY(pnt));
  }
  return res;
}

void Autonomous::autonomyIter() 
{
  transform_t gps = readGPS();
  transform_t odom = readOdom();
  points_t lidar = readLidarScan();
  points_t landmarks = readLandmarks();

  double dtheta = pathDirection(lidar, gps);
  double speed = 10.0;
  // TODO incredibly clever algorithms for state estimation
  // and path planning and control!
  
  if(!lidar.empty()) {
    speed = 5.0;
  }
  setCmdVel(dtheta, speed);
}

double Autonomous::pathDirection(points_t lidar, transform_t gps) {
    double dtheta;
    if(lidar.empty()) {
        dtheta = 0.0;
    } else {
        pose_t gpsPose = toPose(gps, 0);
        const std::vector<PointXY>& ref = points_tToPointXYs(lidar);
        PointXY direction = pather.getPath(ref, static_cast<float>(gpsPose(0)), static_cast<float>(gpsPose(1)), target);
        float theta = atan(direction.y / direction.x);
        dtheta = static_cast<double>(theta) - gpsPose(2);
    }
    return dtheta;
}


void Autonomous::setWorldData(std::shared_ptr<WorldData> wdata)
{
    this->worldData = wdata;
}

std::pair<float, float> Autonomous::getDirections(float currHeading)
{
    std::pair<float, float> directions; // heading, speed
    if (state == 1)
    {
        return stateForwards(currHeading, directions);
    }
    if (state == 0)
    {
        return stateTurn(currHeading, directions);
    }
    if (state == -1)
    {
        return stateBackwards(currHeading, directions);
    }
}

std::pair<float, float>
Autonomous::stateForwards(float currHeading,
                          std::pair<float, float> directions)
{
    float speed = worldData->targetDistance();
    if (!worldData->lidarSees() || speed != 1)
    { // no obstacles in front
        if (forwardCount > 0 || forwardCount < 0)
        { //move forward
            directions = std::make_pair(currHeading, speed);
            state = 1;
            forwardCount--;
            return directions;
        }
        else
        { //moved forwards for a set number of times, now turn
            forwardCount = -1;
            rightTurn = false; //moved forward enough times without obstruction, next turn to target
            return stateTurn(currHeading, directions);
        }
    }
    else
    { // lidar now sees something
        return stateBackwards(currHeading, directions);
    }
}

std::pair<float, float>
Autonomous::stateTurn(float currHeading, std::pair<float, float> directions)
{
    if ((state == 0) && (currHeading == targetHeading))
    { // done turning
        return stateForwards(currHeading, directions);
    }
    else
    { // find heading to turn to
        if (rightTurn || forwardCount > 0)
        { // turn right if last turn was towards target or was obstructed when moving forwards
            targetHeading = currHeading + 45;
            forwardCount = 5; //reset forward count
        }
        else
        { //calculate angle to obstacle
            PointXY robotPos = worldData->getGPS();
            float x = target.x - robotPos.x;
            float y = target.y - robotPos.y;
            targetHeading = atan2f(y, x) * (180 / PI);
            targetHeading = 360 - targetHeading + 90;
            rightTurn = true; //next turn will turn right
        }
        if (targetHeading > 360)
        { //fixing angles over 360
            targetHeading = targetHeading - 360;
        }
        directions = std::make_pair(targetHeading, 0);
        state = 0;
        return directions;
    }
}

std::pair<float, float>
Autonomous::stateBackwards(float currHeading,
                           std::pair<float, float> directions)
{
    if (worldData->lidarSees())
    { // back up
        directions = std::make_pair(currHeading, -1);
        state = -1;
        return directions;
    }
    else
    { // done backing up, now turn
        return stateTurn(currHeading, directions);
    }
}

PointXY Autonomous::getTarget()
{
    return target;
}

