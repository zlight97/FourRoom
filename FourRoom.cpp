#include <WMtk.h>
#include "FourRoom.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

#define atActor info.at(agentY)[agentX]
#define upActor info.at(agentY-1)[agentX]
#define downActor info.at(agentY+1)[agentX]
#define leftActor info.at(agentY)[agentX-1]
#define rightActor info.at(agentY)[agentX+1]

using namespace std;

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
    time_t random_seed = time(NULL);
    srand(random_seed);
    srand48(random_seed);

    state current_state;
    current_state.initState();
    char c = '1';
    while(c!='0')
    {
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
