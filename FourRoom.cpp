#include <WMtk.h>
#include "FourRoom.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

//macros that simplified writting the state class
#define atActor info.at(agentY)[agentX]
#define upActor info.at(agentY-1)[agentX]
#define downActor info.at(agentY+1)[agentX]
#define leftActor info.at(agentY)[agentX-1]
#define rightActor info.at(agentY)[agentX+1]

using namespace std;

/*
 * How will the Higharchy work?
 * Top layer:
 * Recieves reward from getting key and lock (-1 for each step)
 * Needs to learn how to dictate waypoints to lower level (doors, key, and lock)
 * State function gives location of actor, (x,y) and nothing else
 * 
 * Bottom Layer:
 * Forced 1 size memory
 * Chooses a direction (Up down left right) -> These are it's Direction Chunks
 * Reward is dictated by higher level (I don't know how to do this yet) (-1 for each step could also go here)
 * Needs to learn how to navigate to waypoints dicated by higher level
 * State function is the same as higher level? Maybe chould be changed to incorperate waypoints, but I'm not sure how that would work
 */
string getTileName(Tile t)
{
    switch(t)
    {
        case EMPTY:
            return "EMPTY";
        case WALL:
            return "WALL";
        case KEY:
            return "KEY";
        case LOCK:
            return "LOCK";
        case AGENT:
            return "AGENT";
        default:
            return "???";
    }
}

void printMap(state s)
{
    for(int j = 0; j<totalSize; j++)
    {
        for(int i = 0; i<totalSize; i++)
        {
            switch(s.info[j][i])
            {
                case AGENT:
                cout<<"A ";
                break;
                case WALL:
                cout<<"X ";
                break;
                case KEY:
                cout<<"K ";
                break;
                case LOCK:
                cout<<"L ";
                break;
                case EMPTY:
                cout<<". ";
                break;
                default:
                break;
            }
        }
        cout<<"\n";
    }
}

void RunSimulation(bool verbose, bool end)
{

    // double finished_percentage = .99;
    int number_of_trials = 10000;

    //this block holds the settings for the success window
    // int window_size = 20;//this will succed with higher values, though it will take a longer time
    // double goodness = 0.;
    // int goodness_index = 0;
    // int window[window_size];
    // int q; //counter for sliding window
    // for(q=0;q<window_size;q++) //initilizing window
    //     window[q]=0;

    int wm_size = 3;
    int state_feature_vector_size = 2*totalSize;
    // int chunk_feature_vector_size = 6;
    // double lrate = .01;
    // double lambda = .7;
    // double ngamma = .99;
    // double exploration_percentage = .01;//learns faster with ~.05, but more consistant with .01
    time_t random_seed = time(NULL);
    srand(random_seed);
    srand48(random_seed);
    cerr<< "Random Seed is: "<<random_seed<<"\n";
    state current_state;//this is the state data-type
    bool use_actor = false;//this isn't implemented in WMTK
    OR_CODE or_code = NOISY_OR;//read WMTK's FeatureVector.h for more info
                               //I would get best results with NOISY, then MAX, then NO


    current_state.initState();
    char c = '1';
    while(c!='0')
    {
        distanceClear d = current_state.getDistanceClear();
        cout<<d.up<<" "<<d.down<<" "<<d.left<<" "<<d.right<<endl;
        printMap(current_state);
        cin>>c;
        if(c=='w')
            current_state.moveUp();
        if(c=='a')
            current_state.moveLeft();
        if(c=='s')
            current_state.moveDown();
        if(c=='d')
            current_state.moveRight();
    }
    printMap(current_state);
}

/*
 * This is layed out in a manner that should give a spread based on available options
 * 1 is where we are, .6 is where we can move .3 is where we can move in 2 moves
 * Shown by this graphic:
 * 0  0 .3  0  0
 * 0 .3 .6 .3  0
 *.3 .6  1 .6 .3
 * 0 .3 .6 .3  0
 * 0  0 .3  0  0
 */
void stateFunctionBoth(FeatureVector& fv, WorkingMemory& wm)
{
    fv.clearVector();
    state *current_state =(state*) wm.getStateDataStructure();
    int x = current_state->getAgentX();
    int y = current_state->getAgentY();
    fv.setValue(x,1.);
    fv.setValue(y,1.);
    distanceClear c = current_state->getDistanceClear();
    //DIAGNOLS are not yet implemnted - this cant handle that I don't think
    if(c.left>=1)
        fv.setValue(x-1,.6);
    if(c.left>=2)
        fv.setValue(x-2,.3);
    if(c.right>=1)
        fv.setValue(x+1,.6);
    if(c.right>=2)
        fv.setValue(x+2,.3);
    if(c.up>=1)
        fv.setValue(y-1,.6);
    if(c.up>=2)
        fv.setValue(y-2,.3);
    if(c.down>=1)
        fv.setValue(y+1,.6);
    if(c.down>=2)
        fv.setValue(y+2,.3);

}






//State Class Functions:
void state::initState()
{
    for(int i = 0; i<totalSize; i++)
    {
        info.push_back(vector<Tile>());
        for(int j = 0; j<totalSize;j++)
        {
            if(j == totalSize/2 || j==(totalSize/2)-1||i == totalSize/2 || i==(totalSize/2)-1)
                info.at(i).push_back(WALL);
            else
                info.at(i).push_back(EMPTY);
        }
    }
    info.at(lockY)[lockX] = LOCK;
    info.at(keyY)[keyX] = KEY;
    while(true)
    {   
        int r1 = rand()%totalSize;
        int r2 = rand()%totalSize;
        if(info.at(r1)[r2]==EMPTY)
        {
            info.at(r1)[r2]=AGENT;
            agentX = r2;
            agentY = r1;
            break;
        }
    }
    //DOORS GO HERE
    //Left
    info.at(roomSize+1)[2] = EMPTY;
    info.at(roomSize)[2] = EMPTY;

    //right
    info.at(roomSize+1)[roomSize+2] = EMPTY;
    info.at(roomSize)[roomSize+2] = EMPTY;

    //top
    info.at(1)[roomSize+1] = EMPTY;
    info.at(1)[roomSize] = EMPTY;

    //bottom
    info.at(roomSize+5)[roomSize+1] = EMPTY;
    info.at(roomSize+5)[roomSize] = EMPTY;
    
}

bool state::moveUp()
{
    if(agentY==0)
        return 0;

    switch(upActor)
    {
        case EMPTY:
    cout<<getTileName(getAgentTileData())<<endl;
            upActor=AGENT;
            atActor=getAgentTileData();
        break;
        case LOCK:
            upActor=AGENT;
            atActor=getAgentTileData();
            if(acquiredKey)
                success = true;
        break;
        case KEY:
            upActor=AGENT;
            atActor=getAgentTileData();
            acquiredKey = true;
        break;

        case WALL:
            return 0;
        default:
            return 0;
    }
    agentY--;
    return 1;
}

bool state::moveDown()
{
    if(agentY==totalSize-1)
        return 0;

    switch(downActor)
    {
        case EMPTY:

    cout<<getTileName(getAgentTileData())<<endl;
            atActor=getAgentTileData();
            downActor=AGENT;
        break;
        case LOCK:
            downActor=AGENT;
            atActor=getAgentTileData();
            if(acquiredKey)
                success = true;
        break;
        case KEY:
            downActor=AGENT;
            atActor=getAgentTileData();
            acquiredKey = true;
        break;

        case WALL:
            return 0;
        default:
            return 0;
    }
    agentY++;
    return 1;
}

bool state::moveLeft()
{
    if(agentX==0)
        return 0;

    switch(leftActor)
    {
        case EMPTY:
            leftActor=AGENT;
            atActor=getAgentTileData();
        break;
        case LOCK:
            leftActor=AGENT;
            atActor=getAgentTileData();
            if(acquiredKey)
                success = true;
        break;
        case KEY:
            leftActor=AGENT;
            atActor=getAgentTileData();
            acquiredKey = true;
        break;

        case WALL:
            return 0;
        default:
            return 0;
    }
    agentX--;
    return 1;
}

bool state::moveRight()
{
    if(agentX==totalSize-1)
        return 0;

    switch(rightActor)
    {
        case EMPTY:
            rightActor=AGENT;
            atActor=getAgentTileData();
        break;
        case LOCK:
            rightActor=AGENT;
            atActor=getAgentTileData();
            if(acquiredKey)
                success = true;
        break;
        case KEY:
            rightActor=AGENT;
            atActor=getAgentTileData();
            acquiredKey = true;
        break;

        case WALL:
            return 0;
        default:
            return 0;
    }
    agentX++;
    return 1;
}

Tile state::getAgentTileData()
{
    if(agentX==lockX&&agentY==lockY)
        return LOCK;
    if(agentY==keyY&&agentX==keyX)
        return KEY;
    return EMPTY;
}

double state::checkLocation()
{//this will be used to determine the given reward to the upper level while updating state info
    if(atActor.getAgentTileData() == KEY && !hasKey())
    {
        acquiredKey = 1;
        return 20.;
    }
    if(atActor.getAgentTileData()==LOCK && hasKey())
    {
        success = 1;
        return 100.;
    }
    return -1.;
}

distanceClear state::getDistanceClear()
{
//Top
    distanceClear ret;

    if(agentY<2)
    {
        if(agentY==1&&upActor==WALL)
            ret.up = 0;
        else ret.up = agentY;
    }
    else if(upActor==WALL)
        ret.up = 0;
    else if(info[agentY-2][agentX]==WALL)
        ret.up = 1;
    else ret.up = 2;

//Bottom
    if(agentY>totalSize-3)
    {
        if(agentY==totalSize-2&&downActor==WALL)
            ret.down = 0;
        else ret.down = totalSize-agentY-1;
    }
    else if(downActor==WALL)
        ret.down = 0;
    else if(info[agentY+2][agentX]==WALL)
        ret.down = 1;
    else ret.down = 2;

//Left side
    if(agentX<2)
    {
        if(agentX==1&&leftActor==WALL)
            ret.left = 0;
        else ret.left = agentX;
    }
    else if(leftActor==WALL)
        ret.left = 0;
    else if(info[agentY][agentX-2]==WALL)
        ret.left = 1;
    else ret.left = 2;

//Right Side
    if(agentX>totalSize-3)
    {
        if(agentX==totalSize-2&&rightActor==WALL)
            ret.right = 0;
        else ret.right = totalSize-agentX-1;
    }
    else if(rightActor==WALL)
        ret.right = 0;
    else if(info[agentY][agentX+2]==WALL)
        ret.right = 1;
    else ret.right = 2;


    return ret;
}