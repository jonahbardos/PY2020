#pragma once
#include "EnvMap.h"
#include <vector>

// make reference or pointer to global SLAM map obj
// recieve list of obstacles within specified range
// plot obstacles onto 2d grid
// pass grid onto pathing
// recreate grid as orientation/location changes

// increase bounding box

class PathMap
{
	vector<vector<bool>> pathMap;

private:
	std::vector<std::shared_ptr<MapObstacle>> getData();

public:
	void updatePathMap();
	void PathMap();
}