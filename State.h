#ifndef STATE_H
#define STATE_H

#include <WMtk.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#define atActor info.at(agentY)[agentX]
#define upActor info.at(agentY-1)[agentX]
#define downActor info.at(agentY+1)[agentX]
#define leftActor info.at(agentY)[agentX-1]
#define rightActor info.at(agentY)[agentX+1]


//ideally once this is done it should be written in a manner that roomSize 
//can be changed and everything should scale 
//(currently not the case as walls are hardcoded)
const int roomSize = 5;
const int totalSize = (roomSize*2)+2;
const int keyX = totalSize-3;
const int keyY = totalSize-roomSize;
const int lockX = 4;
const int lockY = totalSize-2;

const int stateStepAllowance = 100;


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
    distanceClear getDistanceClear(bool b) const;
    distanceClear getDistanceClear(int x, int y) const;
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
    int getSteps() {return steps;}
    void step() {steps++;}
    void resetSteps() {steps = 0;}
private:
    Tile getAgentTileData();
    Goal currentGoal;
    bool acquiredKey = false;
    bool success = false;
    bool reachedGoal = false;
    int agentX;
    int agentY;
    int steps;
};

#endif