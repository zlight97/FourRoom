#ifndef FOURROOM_H
#define FOURROOM_H
#include <WMtk.h>
#include <vector>

//ideally once this is done it should be written in a manner that roomSize 
//can be changed and everything should scale 
//(currently not the case as walls are hardcoded)
const int roomSize = 5;
const int totalSize = (roomSize*2)+2;
const int keyX = totalSize-3;
const int keyY = totalSize-roomSize;
const int lockX = 4;
const int lockY = totalSize-2;


//Each tile has a type dicated by this enum
enum Tile
{
    AGENT,
    WALL,
    KEY,
    LOCK,
    EMPTY
};

//Direction type for navigation chunks (lower level)
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    INVALID
};

struct Goal
{
    int x;
    int y;
};

struct distanceClear
{
    int up=0;
    int down=0;
    int left=0;
    int right=0;
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
    distanceClear getDistanceClear(bool b);
    double checkLocation();
    int getAgentX() const{ return agentX; }
    int getAgentY() const{ return agentY; }
    bool getSuccess() { return success; }
    bool hasKey() { return acquiredKey; }
    bool goalReached() {return reachedGoal;}
    void atGoal() {reachedGoal = 1;}
    void setGoal(int x, int y) {currentGoal.x = x; currentGoal.y = y; reachedGoal = 0;}
    Goal getCurrentGoal() {return currentGoal;}
    bool hitWall = false;
private:
    Tile getAgentTileData();
    Goal currentGoal;
    bool acquiredKey = false;
    bool success = false;
    bool reachedGoal = false;
    int agentX;
    int agentY;
};


// The main function for the program. This is the function that actually
// performs the simulation.
void RunSimulation(bool verbose=false, bool end=false);

//This outputs the string equivilent from a Tile
string getTileName(Tile t);
//Prints the map in cout
void printMap(state s);

void populateMoveChunkList(list<Chunk> &lst);
void populateGoalChunkList(list<Chunk> &lst,const state &s);

//Both AI
void deleteChunkFunction(Chunk& chk);

//Higher Level:
void upperStateFunction(FeatureVector& fv, WorkingMemory& wm);
double upperRewardFunction(WorkingMemory& wm);
void upperChunkFunction(FeatureVector& fv, Chunk& chk, WorkingMemory& wm);

//Lower Level:
void lowerStateFunction(FeatureVector& fv, WorkingMemory& wm);
double lowerRewardFunction(WorkingMemory& wm);
void lowerChunkFunction(FeatureVector& fv, Chunk& chk, WorkingMemory& wm);

#endif