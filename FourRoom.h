#ifndef FOURROOM_H
#define FOURROOM_H
#include <WMtk.h>
#include <vector>
#include "State.h"


// The main function for the program. This is the function that actually
// performs the simulation.
void RunSimulation(bool verbose=false, bool end=false);

//This outputs the string equivilent from a Tile
string getTileName(Tile t);
//Prints the map in cout
void printMap(state s);

//these create the list of valid chunks for each AI to choose from
void populateMoveChunkList(list<Chunk> &lst, const state &s);
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