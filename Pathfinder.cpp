#include <vector>
#include <fstream>
#include <sstream>
#include <set>

#include "Pathfinder.h"

using namespace std;

Pathfinder::Pathfinder() {
    srand(time(nullptr));

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 5; k++) {
                maze[k][j][i] = 1; // Initialize maze to all ones
            }
        }
    }
}

Pathfinder::~Pathfinder() = default;

string Pathfinder::toString() const {
    stringstream writeToMaze;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 5; k++) {
                if (k < 4) {
                    writeToMaze << maze[k][j][i] << " ";
                } else {
                    writeToMaze << maze[k][j][i];
                }
            }
            writeToMaze << endl;
        }
        writeToMaze << endl;
    }

    return writeToMaze.str();
}

void Pathfinder::createRandomMaze() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 5; k++) {
                maze[k][j][i] = rand() % 2; // Randomize zeroes and ones in maze
            }
        }
    }

    maze[0][0][0] = 1;
    maze[4][4][4] = 1;
}

bool Pathfinder::importMaze(string fileName) {
    ifstream inFile;
    int counter = 0;
    int tempMaze[5][5][5]; // Create tempMaze to verify before copying over to maze

    inFile.open(fileName);

    if (inFile.is_open()) {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                for (int k = 0; k < 5; k++) {
                    if (!inFile.eof()) {
                        counter += 1;
                    }

                    inFile >> tempMaze[k][j][i];

                    if ((tempMaze[k][j][i] != 0) && (tempMaze[k][j][i] != 1)) {
                        inFile.close();
                        return false;
                    } else if ((((i == 0) && (j == 0) && (k == 0)) || ((i == 4) && (j == 4) && (k == 4))) && (tempMaze[k][j][i] != 1)) {
                        inFile.close();
                        return false;
                    }
                }
            }
        }

        if (!inFile.eof()) {
						inFile.close();
            return false;
        }

        if ((tempMaze[0][0][0] == 1) && (tempMaze[4][4][4] == 1) && (counter == 125)) {
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    for (int k = 0; k < 5; k++) {
                        maze[k][j][i] = tempMaze[k][j][i];
                    }
                }
            }

            inFile.close();
            return true;
        }
    } else {
				inFile.close();
        return false;
    }

		inFile.close();
		return false;
}

vector<string> Pathfinder::solveMaze() {
    set<string> priorLocations; // Use set to add locations visited before

    path.clear();

    findPath(priorLocations, 0, 0, 0);

    return path;
}

bool Pathfinder::findPath(set<string>& priorLocations, int x, int y, int z) {
    stringstream currentLocation;
    currentLocation << "(" << z << ", " << y << ", " << x << ")";
    string location = currentLocation.str();
    path.push_back(location);

    if (x < 0 || x > 4 || y < 0 || y > 4 || z < 0 || z > 4) {
        path.pop_back();
        return false;
    }

    if (priorLocations.count(location) == 1) { // Return false if the set contains the location
        path.pop_back();
        return false;
    } else {
        priorLocations.insert(location);
    }

    if (maze[z][y][x] == 0) {
        path.pop_back();
        return false;
    }

    if (x == 4 && y == 4 && z == 4) {
        return true;
    }

    if (findPath(priorLocations, x - 1, y, z) || findPath(priorLocations, x + 1, y, z) ||
        findPath(priorLocations, x, y - 1, z) || findPath(priorLocations, x, y + 1, z) ||
        findPath(priorLocations, x, y, z - 1) || findPath(priorLocations, x, y, z + 1)) {
        return true;
    } else {
        path.pop_back();
        return false;
    }
}
