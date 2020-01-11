#include "Map.h"
#include "Pather.h"
#include <vector>
#include <queue>
#include <iostream>

//matrix size
#define ROW 5
#define COL 5

void print_point(std::queue<Pather::point> q)
{
  while (!q.empty())
  {
    std::cout << "(" << q.front().x << ",";
    std::cout << q.front().y << ")";
    q.pop();
  }
  std::cout << std::endl;
}

void print_queue(std::queue<Pather::queueNode> q)
{
  while (!q.empty())
  {
    std::cout << "Hello";
    std::queue<Pather::point> temp = q.front().path;
    print_point(temp);
    std::cout << std::endl;
    q.pop();
  }
  std::cout << std::endl;
}

//check if node is valid
bool isValid(int x, int y) {
    return ((x >= 0) && (x < ROW) && (y >= 0) && (y < COL));
}

int rowNum[] = {-1,0,0,1};
int rowCol[] = {0,-1,1,0};

Pather::point BFS(int mat[ROW][COL], Pather::point src, Pather::point dest){

    //check if src and dest are represented with 1 not 0
    if (!mat[src.x][src.y] || !mat[dest.x][dest.y]){
        struct Pather::point error = {-1, -1};
        return error;
    }

    //create 2D boolean array and use memset to set all values as false
    bool visited[ROW][COL];
    memset(visited, false, sizeof visited); 

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (mat[i][j] == 0) {
                visited[i][j] = true;
            }
        }
    } 

    //set source as visited, create node with dist 0 from src and push to queue of nodes to be checked
    visited[src.x][src.y] = true;
    std::queue<Pather::queueNode> q;
    std::queue<Pather::point> newQueue;
    Pather::queueNode s = {src, 0, newQueue};
    q.push(s);

    

    while (!q.empty()){
        print_queue(q);
        //get front node in nodes to be checked and make it current
        Pather::queueNode curr = q.front();
        Pather::point pt = curr.pt;

        //check if current node is destination node
        if ((pt.x == dest.x) && (pt.y == dest.y)){
            //return first node after source in path to destination
            std::queue<Pather::point> temp = curr.path;
            temp.pop();
            std::cout << "Hi" << std::endl;
            return temp.front();
        }

        //pop current node off nodes to be checked
        q.pop();
 
        //iterate through adj nodes
        for (int i = 0; i < 4; i++){
            int row = pt.x + rowNum[i];
            int col = pt.y + rowCol[i];
            

            //check if adj node is not out of bounds and is not an obstacle
            if (isValid(row, col) && !visited[row][col]){
                //set adj node as visited and push it to nodes to be checked
                struct Pather::point test = {row, col};
                curr.path.push(test);
                Pather::queueNode adjNode = {{row, col}, curr.dist + 1, curr.path};
                visited[adjNode.pt.x][adjNode.pt.y] = true;
                q.push(adjNode);
            }
        }

    }
    return src;
}

int main () {
    int map[ROW][COL] = {{0, 1, 0, 1, 0},
               {0, 0, 0, 0, 1},
               {0, 1, 1, 0, 0},
               {0, 1, 1, 0, 1},
               {1, 1, 1, 1, 1}};
    struct Pather::point src = {2, 2};
    struct Pather::point destination = {4, 4};
    struct Pather::point a = BFS(map, src, destination);
    std::cout << a.x;
    std::cout << a.y;
    std::cout << "" << std::endl;
    return 0;
}