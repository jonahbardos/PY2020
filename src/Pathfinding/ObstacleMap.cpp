#include "ObstacleMap.h"
#include <iostream>
#include <cstdlib>

//gets robot position
inline void ObstacleMap::getRobotPosition(float &robotX, float &robotY)
{
    robotX = 0.0f;//GPS.lat TBD
    robotY = 0.0f;//GPS.long TBD
}


//sets all values in ObstacleMap to false
void ObstacleMap::resetObstacleMap()
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
           ObstacleMap::obstacle_map[i][j] = false;
        }
    }
}

//rebuilds ObstacleMap with given Obstacles
void ObstacleMap::update(std::vector<Point>& obstacles)
{
    float robotX = 0.0f;
    float robotY = 0.0f;
    getRobotPosition(robotX, robotY);
    resetObstacleMap();
    int x, y;
    float px, py;
    for (Point p : obstacles) {
        x = static_cast<int>(p.x - robotX) + radius;
        y = static_cast<int>(p.y - robotY) + radius;
        if (x >= 0 && x < size && y >= 0 && y < 21)
        {
            modifyObstacleMap(x,y);
        }
    }
}

//rounds given coordinates up/down to obstacle_map indices,
//sets four elements around given coordinates as blocked
inline void ObstacleMap::modifyObstacleMap(int x, int y)
{
    if (y + 1 < size && x + 1 < size) {
        obstacle_map[y + 1][x + 1] = true;
    }
    if (y + 1 < size && x >= 0) {
        obstacle_map[y + 1][x] = true;
    }
    if (y >= 0 && x + 1 < size) {
        obstacle_map[y][x + 1] = true;
    }
    if (y >= 0 && x >= 0) {
        obstacle_map[y][x] = true;
    }
}

//for testing purposes only, prints a visual representation of ObstacleMap
//where 1 is an obstacle and 0 is empty
void ObstacleMap::print()
{
    for (int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            if(ObstacleMap::obstacle_map[i][j])
            {
                std::cout << "1 ";
            }else
            {
                std::cout << "0 ";
            }
        }
        std::cout << std::endl;
    } 
}
