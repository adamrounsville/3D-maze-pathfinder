#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Pathfinder.h"

const int NUM_FILES = 5; // The total number of files to be read from

const std::string fileArray[NUM_FILES] = { "file1.txt", "file2.txt", "file3.txt", "file4.txt", "file5.txt" }; // The string array containing the file names

// This will take a string temp and a Pathfinder object and will execute an instruction from the string
// No return value, but writes the results of the instruction into the ofs filestream
void parseInstructions(std::string temp, std::ofstream &ofs, Pathfinder* aptr);

// This function will take a vector containing a suggested path through a maze and determine if the path is valid
// If valid, returns "valid path" else returns the invalid path and a message explaining why the path is invalid
std::string isValidPath(vector<string> &vec, std::string mazeString);

// This function is a platform independent way of reading files of various line ending types
// Its definition is at the bottom of the file, don't worry if you don't understand it
namespace ta {
    std::istream& getline(std::istream& is, std::string& line);
}


int main() {
    std::ifstream ifs; // Create the stream to read in from the files
    std::ofstream ofs; // Create the output stream to write to an output file
    std::string temp; // Used to store the current instruction
    Pathfinder* pathPtr = nullptr; // The Pathfinder

    for (int i = 0; i < NUM_FILES; i++) {
        ifs.open(fileArray[i]); // Open the file to read from
        ofs.open("out_" + fileArray[i]); // Open the file to write to
        pathPtr = new Pathfinder();

        if (!ifs.is_open()) { // If the file did not open, there was no such file
            std::cout << "File " << i + 1 << " could not open, please check your lab setup" << std::endl;
        } else {
            std::cout << "Reading file" << i + 1 << ".txt..." << std::endl;
        }

        std::cout << "Beginning out_file" << i + 1 << ".txt write" << std::endl;
        while (ta::getline(ifs, temp)) { // While there are more instructions to get, parse the instructions using the Pathfinder
            parseInstructions(temp, ofs, pathPtr);
        }
        std::cout << "File write complete" << std::endl << std::endl;
        if (pathPtr != nullptr) {
            delete pathPtr;
            pathPtr = nullptr;
        }

        ifs.close();
        ofs.close();
    }
    std::cout << "end" << std::endl; // Indicate that the program has successfully executed all instructions
    return 0;
}

void parseInstructions(std::string temp, std::ofstream &ofs, Pathfinder* aptr) {
    std::string command, expression;
    std::stringstream ss(temp);

    if (!(ss >> command)) {
        return;
    } // Get command, but if string was empty, return
    if (command == "importMaze") { // Command to import a new maze from a file
        if (aptr->importMaze(ss.str().substr(11, std::string::npos))) { // Use the rest of the stringstream as importMaze input
            ofs << temp << " True" << std::endl;
        } else {
            ofs << temp << " False" << std::endl;
        }
    } else if (command == "toString") { // Command to return the maze represented as a string
        ofs << temp << "\n" << aptr->toString();
    } else if (command == "createRandomMaze") { // Command to create a random maze
        stringstream ss;
        aptr->createRandomMaze(); // create the maze
        vector<std::string> pathVec = aptr->solveMaze(); // Attempt to solve the maze;

        if (pathVec.empty()) { // No valid path, maze is unsolvable
            ofs << temp << "\n" << aptr->toString() << "\nPath: Unsolvable\n";
        } else {
            ofs << temp << "\n" << aptr->toString() << std::endl << isValidPath(pathVec, aptr->toString()) << std::endl; //<< ":\n" << ss.str() << std::endl;
        }
    } else if (command == "solveMaze") { // Command to solve the maze and return a correct path if possible
        std::string originalMaze = aptr->toString();
        vector<std::string> pathVec = aptr->solveMaze(); // Get the path vector returned by the student

        if (originalMaze != aptr->toString()) {
            ofs << temp << "\nINVALID solution. Maze has been altered from: \n" << originalMaze
                << "\n...to: \n\n" << aptr->toString() << std::endl;
        } else {
            ofs << temp << " " << isValidPath(pathVec, aptr->toString()) << std::endl;
        }

    } else { // Invalid command, wrong input file format
        std::cout << "Command: \"" << command << "\"" << std::endl;
        std::cout << "Invalid command.  Do you have the correct input file?" << std::endl;
    }
}


// Insert values for x, y, and z into the given ints based on the coordinates given in coordString
// Returns "" if the string given was a valid coordinate, else returns the coordString
string getCoords(std::string coordString, int &x, int &y, int &z) {
    stringstream ss;

    if (coordString.size() != 9) { // Coordinate string must contain 9 characters "(x, y, z)"
        return coordString;
    }

    ss << coordString.at(1) << " "; // Grab the x, y, and z coordinates
    ss << coordString.at(4) << " ";
    ss << coordString.at(7) << " ";

    if (ss >> x && ss >> y && ss >> z) { // If each coordinate is a valid int, return "" indicating a valid coordinate
        return "";
    }

    return coordString; // Else invalid coordinate given
}

// This function will take a vector containing a suggested path through a maze and determine if the path is valid
// If valid, returns "valid path" else returns the invalid path and a message explaining why the path is invalid
std::string isValidPath (vector<string> &vec, std::string mazeString) {
    char maze[125];
    stringstream ss(mazeString);
    stringstream pathSS; // Will hold a string representation of the path given in vec

    pathSS << "Path Given:\n";
    for (int i = 0; i < 125; i++) { // Import the maze information
        ss >> maze[i];
    }

    if (vec.empty()) {
        return "Maze has no valid solution";
    }

    for (int i = 0; i < vec.size(); i++) { // Load the maze string into pathSS
        pathSS << vec[i] << std::endl;
    }

    if (vec.size() > 0 && vec[0] != "(0, 0, 0)") {
        return "INVALID path given, path must begin at cell (0, 0, 0)\n" + pathSS.str();
    }

    if (vec[vec.size()-1] != "(4, 4, 4)") {
        return "INVALID path given, path must end at cell (4, 4, 4)\n" + pathSS.str();
    }

    int xloc = -1, yloc = 0, zloc = 0; // Used to hold the previous location of x, y, and z. Begin at x = -1 to indicate that no path has yet been traversed

    for (int i = 0; i < vec.size(); i++) { // For each of the coordinates given:
        int newXLoc, newYLoc, newZLoc; // Will hold the new location of x, y, and z
        int xMov = 0, yMov = 0, zMov = 0; // Will be 1 if the given coordinate changes from last time, else 0

        if (getCoords(vec[i], newXLoc, newYLoc, newZLoc) != "") { // Store the new coords and determine if coord was valid
            return "INVALID coordinate format found: " + vec[i];
        }

        (xloc == newXLoc) ? xMov = 0 : xMov = abs(xloc - newXLoc); // If x has changed, increment xMov
        (yloc == newYLoc) ? yMov = 0 : yMov = abs(yloc - newYLoc); // If y has changed, increment yMov
        (zloc == newZLoc) ? zMov = 0 : zMov = abs(zloc - newZLoc); // If z has changed, increment zMov

        if (xMov + yMov + zMov != 1) { // If more than one coordinate value has changed,
            return "INVALID path given, path must move through one adjacent cell at a time\n" + pathSS.str();
        }

        if (newXLoc < 0 || newXLoc > 4 || newYLoc < 0 || newYLoc > 4 || newZLoc < 0 || newZLoc > 4) {
            return "INVALID path given, coordinates must represent a cell within the maze\n" + pathSS.str();
        }

        if (maze[newXLoc + (5 * newYLoc) + (25 * newZLoc)] != '1') { // Coord was not on a valid path
            cout << newXLoc << " " << newYLoc << " " << newZLoc << " " << maze[newXLoc + (5 * newYLoc) + (25 * newZLoc)] << endl;
            return "INVALID path given, path attempted to go through a blocked cell\n" + pathSS.str();
        }

        xloc = newXLoc; // Update xloc
        yloc = newYLoc; // Update yloc
        zloc = newZLoc; // Update zloc
    }

    return "VALID solution given";
}

// Version of getline which does not preserve end of line characters
namespace ta {
    std::istream& getline(std::istream& in, std::string& line) {
        line.clear();

        std::istream::sentry guard(in, true); // Use a sentry to maintain the state of the stream
        std::streambuf *buffer = in.rdbuf();  // Use the stream's internal buffer directly to read characters
        int c = 0;

        while (true) { // Continue to loop until a line break if found (or end of file)
            c = buffer->sbumpc(); // Read one character
            switch (c) {
                case '\n': // Unix style, return stream for further parsing
                    return in;
                case '\r': // Dos style, check for the following '\n' and advance buffer if needed
                    if (buffer->sgetc() == '\n') { buffer->sbumpc(); }
                    return in;
                case EOF:  // End of File, make sure that the stream gets flagged as empty
                    in.setstate(std::ios::eofbit);
                    return in;
                default:   // Non-linebreak character, go ahead and append the character to the line
                    line.append(1, (char)c);
            }
        }
    }
}