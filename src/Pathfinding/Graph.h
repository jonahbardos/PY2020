#pragma once
#include "Geometry.h"
#include <vector>
// using node_ptr = std::shared_ptr<Node>;

// //struct Neighbor {
//     // x and y are -1 if the point doesn't exist
//     int xIndex;
//     int yIndex;
//     int distance;
// }

// Construct an edge
struct Edge
{
	double distance;
}

// construct a Node
struct Node
{
	// int currX;
	// int currY;
	// Neighbor forwards;
	// Neighbor backwards;
	// Neighbor left;
	// Neighbor right;
	// int distance;
	int x;
	int y;
	struct Node *north;
	struct Node *east;
	struct Node *south;
	struct Node *west;
}

class Graph
{
	int
		// public:

		//     // int radius = 20;
		//     // int step = 1;
		//     // std::vector<Node> map;
		//     // Node robot_current_location;
		//     // void updateRobotLocation(Node newPoint);
		//     // void createNode(Point p);
		//     // double distanceBetweenNodes(node_ptr a, node_ptr b);

		//     // Node getNode(Point p);
		//     // Map();
		struct Node robot_current_location;
	void createNode;
};
