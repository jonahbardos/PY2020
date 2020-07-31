#include "Autonomous.h"
#include "simulator/world_interface.h"
#include <cmath>
#include <Eigen/LU>

constexpr float PI = M_PI;

Autonomous::Autonomous(PointXY _target) : target(_target)
{
    state = 0;
    targetHeading = -1;
    forwardCount = -1;
    rightTurn = false;
    currHeading = 0;
}


PointXY Autonomous::point_tToPointXY(point_t pnt){
    return PointXY{static_cast<float>(pnt(0)), static_cast<float>(pnt(1))};
}

bool Autonomous::arrived(pose_t gpsPose) {
    double currX = gpsPose(0);
    double currY = gpsPose(1);
    return util::almostEqual(currX, (double) target.x, 0.5) &&
            util::almostEqual(currY, (double) target.y, 0.5);
}

const std::vector<PointXY> Autonomous::points_tToPointXYs(points_t pnts) {
  std::vector<PointXY> res;
  for(point_t &pnt : pnts) {
    res.push_back(point_tToPointXY(pnt));
  }
  return res;
}

double Autonomous::angleToTarget(pose_t gpsPose) {
    float dy = target.y - (float) gpsPose(1);
    float dx = target.x - (float) gpsPose(0);
    double theta = (double) atan2(dy, dx);
    return theta - gpsPose(2);
}

void Autonomous::autonomyIter() 
{
  transform_t gps = readGPS(); // <--- has some heading information
  transform_t odom = readOdom();
  points_t lidar = readLidarScan();
  points_t landmarks = readLandmarks();


  pose_t gpsPose = toPose(gps, currHeading);
  currHeading = gpsPose(2);

  // TODO incredibly clever algorithms for state estimation
  // and path planning and control!
  //
  // Ben: better localization than just GPS
  //    EKF that outputs transforms into a map frame
  //  input:
  //    GPS reading, odom reading, old EKF estimate
  //  output:
  //    robot frame -> map frame transform
  //    GPS frame -> map frame transform
  //
  // Jonah: map compositing
  //    taking data from different points in time and combining
  //    into a single map
  //  input:
  //    current lidar scan, past composite list of lidar scans
  //    robot frame -> map frame transform
  //  output:
  //    map (provided as list of points)
  //    list of locations in map frame for the AR tags we've seen so far
  //
  // Atharva: "mission planning" -- higher level planning
  //    driving through gates
  //    deciding when we're close enough to posts
  //  input: internal state, current URCLeg information
  //    currently visible AR tags in robot frame
  //    robot frame -> map frame transform
  //    GPS frame -> map frame transform
  //  output: goal pose in map frame

  URCLeg leg = getLeg(0); // Leg number depends on input?
  
  bool left_post_visible = (landmarks[leg.left_post_id](2) != 0); 
  bool is_gate = leg.right_post_id != -1;
  bool right_post_visible = (is_gate && landmarks[leg.right_post_id](2) != 0);
  
  if (left_post_variable) {
    point_t left_location = landmarks[leg.left_post_id]; // in robot frame
    transform_t tf = gps.inverse();
    point_t left_in_map_frame = tf * left_location;

    if (right_post_visible) {
      // mid-point formula: ((x_l + x_r)/2, (y_l, y_r)/2)
      // Add perpendicular offset to midpoint
      //     Formula:
      //     (U/(distance from left post to mp))*(vector which is perpendicular to gates)
      point_t right_location = landmarks[leg.right_post_id];
      point_t right_in_map_frame = tf * right_location;
      
    }
    else {
      target.x = left_in_map_frame(0);
      target.y = left_in_map_frame(1);
    }
  } else {
    target.x = leg.approx_GPS(0); // Currently just uses GPS frame of reference
    target.y = leg.approx_GPS(1);
  }

  //
  // Assaf: taking a map and computing a plan
  //  input: pose of robot in map frame, map (provided as a list of points), goal pose in map frame
  //  output: movement command
  //

  if (arrived(gpsPose)) { //toPose(gps, currHeading)
    std::cout << "arrived at gate" << std::endl;
    std::cout << "x: " << gpsPose(0) << " y: " << gpsPose(1) << " theta: " << gpsPose(2) << std::endl;


    // implement moving through gates
    // then update target
  } else {
    double dtheta = pathDirection(lidar, gpsPose);
    double speed = 10.0;
    // TODO incredibly clever algorithms for state estimation
    // and path planning and control!
    
    if(!lidar.empty()) {
        speed = 5.0;
    }
    setCmdVel(dtheta, speed);
  }
}

double Autonomous::pathDirection(points_t lidar, pose_t gpsPose) {
    double dtheta;
    if(lidar.empty()) {
        dtheta = angleToTarget(gpsPose);
    } else {
        const std::vector<PointXY>& ref = points_tToPointXYs(lidar);
        PointXY direction = pather.getPath(ref, (float) gpsPose(0), (float) gpsPose(1), target);
        float theta = atan2(direction.y, direction.x);
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

