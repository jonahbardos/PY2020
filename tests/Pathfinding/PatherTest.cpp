#include <catch2/catch.hpp>
#include <iostream>
#include <vector>
#include "../../src/math/PointXY.h"
#include "../../src/Pathfinding/Pather2.h"
#include "../../src/Pathfinding/ObstacleMap.h"

#define CATCH_CONFIG_MAIN

namespace Pathfinding
{
    bool nextPointEqual(PointXY given, PointXY expect) {
        return given.x == expect.x && given.y == expect.y;
    }
}

TEST_CASE("Pathfinding")
{
    std::vector<PointXY> obstaclePosition= {
        PointXY{1.0f, 5.0f},
        PointXY{2.0f, 5.0f},
        PointXY{3.0f, 6.0f},
        PointXY{4.0f, 5.0f},
        PointXY{5.0f, 4.0f},
        PointXY{6.0f, 4.0f},
        PointXY{7.0f, 4.0f},
        PointXY{6.0f, 3.0f}
    };
    //Construct an obstacle map
    ObstacleMap obj;
    obj.update(obstaclePosition, 0.0f, 0.0f);
    
    //Construct a searching path object
    Pather2 searchingPath;
    PointXY nextPoint = searchingPath.mainBFS(obj.obstacle_map, PointXY{10.0f, 10.0f});

    PointXY expectPoint = {1.0f, 0.0};
    //Check if returned next point is as expected.
    REQUIRE(Pathfinding::nextPointEqual(nextPoint, expectPoint));
}