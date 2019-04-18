#include "State.h"
#include <climits>

//State Class Functions:
void state::initState()
{
    for(int i = 0; i<info.size();i++)
    {
        info.at(i).clear();
    }
    if(info.size()>0)
        info.clear();
    for(int i = 0; i<totalSize; i++)
    {
        info.push_back(vector<Tile>());
        for(int j = 0; j<totalSize;j++)
        {
            // if(j == totalSize/2 || j==(totalSize/2)-1||i == totalSize/2 || i==(totalSize/2)-1)
            //     info.at(i).push_back(WALL);
            // else
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
    // info.at(roomSize+1)[2] = EMPTY;
    // info.at(roomSize)[2] = EMPTY;

    // //right
    // info.at(roomSize+1)[roomSize+2] = EMPTY;
    // info.at(roomSize)[roomSize+2] = EMPTY;

    // //top
    // info.at(1.)[roomSize+1] = EMPTY;
    // info.at(1)[roomSize] = EMPTY;

    // //bottom
    // info.at(roomSize+5)[roomSize+1] = EMPTY;
    // info.at(roomSize+5)[roomSize] = EMPTY;
    
    acquiredKey = 0;
    success = 0;
    reachedGoal = 0;
    hitWall = 0;
    steps = INT_MAX;
    currentGoal.x = 0;
    currentGoal.y = 0;
}

bool state::moveUp()
{
    if(agentY==0)
        return 0;

    switch(upActor)
    {
        case EMPTY:
    // cout<<getTileName(getAgentTileData())<<endl;
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

    // cout<<getTileName(getAgentTileData())<<endl;
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
    if(getAgentTileData() == KEY && !hasKey())
    {
        acquiredKey = 1;
        return 20.;
    }
    if(getAgentTileData()==LOCK && hasKey())
    {
        success = 1;
        return 100.;
    }
    return 0.;
}

distanceClear state::getDistanceClear(bool b) const
{
    int agentY = this->agentY;
    int agentX = this->agentX;
    if(b)
    {
        agentY = currentGoal.y;
        agentX = currentGoal.x;
        if(agentX>totalSize||agentX<0)
            agentX = 0;
        if(agentY>totalSize||agentY<0)
            agentY = 0;
            
    }
    
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

distanceClear state::getDistanceClear(int x, int y) const
{
    int agentY = x;
    int agentX = y;

    
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