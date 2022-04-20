#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "PathfinderInterface.h"

using namespace std;

class Pathfinder : public PathfinderInterface {
public:
    Pathfinder();
    ~Pathfinder() override;
    string toString() const override;
    void createRandomMaze() override;
    bool importMaze(string fileName) override;
    bool findPath(set<string>& priorLocations, int x, int y, int z);
    vector<string> solveMaze() override;
private:
    int maze[5][5][5];
    vector<string> path;
};
