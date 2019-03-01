#ifndef FOURROOM_H
#define FOURROOM_H
#include <WMtk.h>
#include <vector>

const int roomSize = 5;
const int totalSize = (roomSize*2)+2;
const int keyX = totalSize-3;
const int keyY = totalSize-roomSize;
const int lockX = 4;
const int lockY = totalSize-2;

enum Tile
{
    AGENT,
    WALL,
    KEY,
    LOCK,
    EMPTY
};

//This holds all of the state data that is used to determine what is currently happening
class state
{
public:
    std::vector <vector<Tile>>info;
    void initState();
    bool moveUp();
    bool moveDown();
    bool moveLeft();
    bool moveRight();
    int getAgentX() {return agentX; }
    int getAgentY() { return agentY; }
    bool getSuccess() { return success; }
    bool hasKey() { return acquiredKey; }
private:
    bool acquiredKey = false;
    bool success = false;
    Tile getAgentTileData();
    int agentX;
    int agentY;
};


// The main function for the program. This is the function that actually
// performs the simulation.
void RunSimulation(bool verbose=false, bool end=false);

void initState(state &s);
void printMap(state s);
#endif