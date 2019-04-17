#include <WMtk.h>
#include "FourRoom.h"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <AggregateFeatureVector.h>

//macros that simplified writting the state class

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
 * 
 * 
 * Questions for later
 * Is there a better way to represent the state vector for the lower level, how should it know where the destination is / maybe upper level?
 * ^Yes, both as two x,y vectors -> x,y,x`,y`
 * Should I force the Upper AI to choose a new goal each time (within range? Limit possibilities?)
 * ^Give a step limit to select new
 * Should I teach the lower to move and then just load it every time? (How should I represent goals in Feature Vector)
 * ^Not a bad idea, same as above
 * How should I be feeding chunks to the AI
 * ^this is fine. Give the upper all valid chunks
 * Is this implementation a tick late?
 * ^Maybe test more
 * Along with that, it appears that the upper-level doesn't get it's reward fuction called
 * ^Test more
 * Should wmtk paramaters be changed
 * ^Changed lambda to 0
 */
int goalCount = 0;
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
            Goal g = s.getCurrentGoal();
            if(g.x == i && g.y == j)
            {
                cout<<"C ";
                continue;
            }
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

void populateMoveChunkList(list<Chunk> &lst, const state &s)
{
    lst.clear();
    Chunk ch;
    ch.setType("MOVE");
    distanceClear dist = s.getDistanceClear(0);
    Direction *d = new Direction();
    if(dist.up>0)
    {
        *d = UP;
        ch.setData(d);
        lst.push_back(ch);
    }
    if(dist.down>0)
    {
        d = new Direction();
        *d = DOWN;
        ch.setData(d);
        lst.push_back(ch);
    }
    if(dist.left>0)
    {        d = new Direction();
        *d = LEFT;
        ch.setData(d);
        lst.push_back(ch);
    }
    if(dist.right>0)
    {
        d = new Direction();
        *d = RIGHT;
        ch.setData(d);
        lst.push_back(ch);
    }
}

void populateGoalChunkList(list<Chunk> &lst,const state &s)
{
    lst.clear();
    Chunk ch;
    ch.setType("GOAL");
    int agentX = s.getAgentX();
    int agentY = s.getAgentY();
    // if(goalCount<100)
    // {
    //     cout<<"goalCountINC\n";
    //     goalCount++;
    //     if(s.getAgentX()>0&&s.leftActor!=WALL)
    //     {
    //         Goal *g = new Goal();
    //         g->x = s.getAgentX()-1; g->y = s.getAgentY();
    //         ch.setData(g);
    //         lst.push_back(ch);
    //     }
    //     if(s.getAgentX()<totalSize-1&&s.rightActor!=WALL)
    //     {
    //         Goal *g = new Goal();
    //         g->x = s.getAgentX()+1; g->y = s.getAgentY();
    //         ch.setData(g);
    //         lst.push_back(ch);
    //     }
    //     if(s.getAgentY()>0&&s.upActor!=WALL)
    //     {
    //         Goal *g = new Goal();
    //         g->x = s.getAgentX(); g->y = s.getAgentY()-1;
    //         ch.setData(g);
    //         lst.push_back(ch);
    //     }
    //     if(s.getAgentY()<totalSize-1&&s.downActor!=WALL)
    //     {
    //         Goal *g = new Goal();
    //         g->x = s.getAgentX(); g->y = s.getAgentY()+1;
    //         ch.setData(g);
    //         lst.push_back(ch);
    //     }
    //     return;
    // }
    // int lB = s.getAgentX()-roomSize/2;
    // lB = lB >=0? lB : 0;
    // int uB = s.getAgentY()-roomSize/2;
    // uB = uB >=0? lB : 0;    
    // int rB = s.getAgentX()+roomSize/2;
    // rB = rB <totalSize? rB : totalSize-1;
    // int dB = s.getAgentY()+roomSize/2;
    // dB = dB <totalSize? dB : totalSize-1;
    // for(int i = uB; i<dB; i++)
    // for(int i = 0; i<totalSize; i++)
    // {
    //     // for(int j = lB; j<rB;j++)
    //     for(int j = 0; j<totalSize; j++)
    //     {
    //         if(s.info.at(i)[j]!=WALL&&s.info.at(i)[j]!=AGENT)
    //         {
    //             Goal *g = new Goal();
    //             g->x = j; g->y = i;
    //             ch.setData(g);
    //             lst.push_back(ch);
    //         }
    //     }
    // }

                Goal *g = new Goal();
                g->x = rand()%totalSize; g->y = rand()%totalSize;
                ch.setData(g);
                lst.push_back(ch);

}

void RunSimulation(bool verbose, bool end)
{

    double finished_percentage = .99;
    int number_of_trials = 100000;
    int steps_per_trial = 100;
    double largestGoodness = 0.;
    //this block holds the settings for the success window
    int window_size = 20;//this will succed with higher values, though it will take a longer time
    double goodness = 0.;
    int goodness_index = 0;
    int window[window_size];
    int q; //counter for sliding window
    for(q=0;q<window_size;q++) //initilizing window
        window[q]=0;

    int lower_wm_size = 1;
    int upper_wm_size = 1;
    int lower_chunk_feature_vector_size = 4;
    int upper_chunk_feature_vector_size = 2*totalSize;
    int upper_state_feature_vector_size = 2*totalSize;
    int lower_state_feature_vector_size = 48;
    //these values go for both
    double lrate = .01;
    double lambda = 0.;//.7;
    double ngamma = .99;
    double exploration_percentage = .05;
    OR_CODE or_code = NOISY_OR;
    state current_state;//this is the state data-type

    time_t random_seed = time(NULL);
    srand(random_seed);
    srand48(random_seed);
    cerr<< "Random Seed is: "<<random_seed<<"\n";
    bool use_actor = false;//this isn't implemented in WMTK

    WorkingMemory WMU(upper_wm_size, upper_state_feature_vector_size,
	upper_chunk_feature_vector_size, &current_state, upperRewardFunction,
	upperStateFunction, upperChunkFunction, deleteChunkFunction, use_actor,
	or_code);
    WorkingMemory WML(lower_wm_size, lower_state_feature_vector_size,
	lower_chunk_feature_vector_size, &current_state, lowerRewardFunction,
	lowerStateFunction, lowerChunkFunction, deleteChunkFunction, use_actor,
	or_code);

    // Use learning rate
	WMU.getCriticNetwork()->setLearningRate(lrate);

	/// Use lambda (eligibility discount rate)
	WMU.getCriticNetwork()->setLambda(lambda);

	// Use gamma (discount rate)
	WMU.getCriticNetwork()->setGamma(ngamma);

	// Use exploration percentage (chance it does something random)
	WMU.setExplorationPercentage(exploration_percentage);

    	// Use learning rate
	WML.getCriticNetwork()->setLearningRate(lrate);

	/// Use lambda (eligibility discount rate)
	WML.getCriticNetwork()->setLambda(lambda);

	// Use gamma (discount rate)
	WML.getCriticNetwork()->setGamma(ngamma);

	// Use exploration percentage (chance it does something random)
	WML.setExplorationPercentage(exploration_percentage);

    Chunk direction_chunk;
    Chunk goalChunk;

    list<Chunk> cannidate_directions;
    list<Chunk> cannidate_goals;

	WMU.saveNetwork("./starting_network_upper.dat");

	WML.saveNetwork("./starting_network_lower.dat");
    
    // char c = '1';
    // while(c!='0')
    // {
    //     distanceClear d = current_state.getDistanceClear();
    //     cout<<d.up<<" "<<d.down<<" "<<d.left<<" "<<d.right<<endl;
    //     printMap(current_state);
    //     cin>>c;
    //     if(c=='w')
    //         current_state.moveUp();
    //     if(c=='a')
    //         current_state.moveLeft();
    //     if(c=='s')
    //         current_state.moveDown();
    //     if(c=='d')
    //         current_state.moveRight();
    // }

        current_state.initState();
    for(int trial = 0; trial<number_of_trials; trial++)
    {
        // if(trial%2000==0&&trial!=0)
        // {
        //     // exploration_percentage -=.8;
	    //     // WML.setExplorationPercentage(exploration_percentage);
        //     lrate += .01;
        //     WML.getCriticNetwork()->setLearningRate(lrate);
        // }
        WMU.newEpisode();
        WML.newEpisode();
        current_state.initState();
        bool stepComplete = 0;
        for(int step = 0; step<steps_per_trial;step++)
        {
            if(current_state.goalReached())
            {
                break;
            }
            if(/*current_state.goalReached()||*/current_state.getSteps()>steps_per_trial)
            {
                populateGoalChunkList(cannidate_goals,current_state);
                // cout<<"UPPER GOALS# "<<cannidate_goals.size()<<" Steps: "<<current_state.getSteps()<<endl;
                WMU.tickEpisodeClock(cannidate_goals);
                if(WMU.getNumberOfChunks()==0)
                {
                    step--;
                    // cout<<"NAH"<<current_state.getSteps()<<endl;
                    continue;
                }
                else
                {
                    goalChunk = WMU.getChunk(0);
                    if(goalChunk.getType()=="GOAL")
                    {
                        stepComplete=1;
                        current_state.resetSteps();
                        Goal* g = (Goal*)goalChunk.getData();
                        current_state.setGoal(g->x,g->y);
                        // cout<<"NEXT GOAL IS: "<<g->x<<", "<<g->y<<endl;
                    }
                }
                
            }
            

            populateMoveChunkList(cannidate_directions,current_state);
            WML.tickEpisodeClock(cannidate_directions);
            
            if(verbose)
            {
                printMap(current_state);
                cout<<endl;
            }
            // if(current_state.getSuccess())
            //     break;
            
        }

        cout<<trial<< " ";
        if (current_state.goalReached()) {
			window[goodness_index++] = 1;
			for (q = 0; q < window_size; q++)
				goodness += (double) window[q];
			goodness /= (double) window_size;

			cout << "1 " << goodness << endl;

		}
		else {
			window[goodness_index++] = 0;
			for (q = 0; q < window_size; q++)
				goodness += (double) window[q];
			goodness /= (double) window_size;

			cout << "0 " << goodness << endl;
		}

		if (goodness_index == window_size)
			goodness_index = 0;

        if(goodness>largestGoodness)
            largestGoodness = goodness;
		// If we are performing as well as we want, then we're finished.
		if (goodness >= finished_percentage) {
			// break;
        }
        // cout<<"FINAL MAP OF TRIAL: "<<trial<<endl<<"SUCESS: "<<current_state.getSuccess()<<" KEY: "<<current_state.hasKey()<<endl;
        // printMap(current_state);
    }
    cout<<"Best Goodness in "<<window_size<<" window size is: "<<largestGoodness<<"\nFinal Map is:\n";
	WMU.saveNetwork("./ending_network_upper.dat");
	WML.saveNetwork("./ending_network_lower.dat");
    printMap(current_state);
}


double upperRewardFunction(WorkingMemory& wm)
{ //could add negative reward for taking too long (bad goal)
    // cout<<"UPPER REWARD CALLED\n";
    state *current_state = (state*) wm.getStateDataStructure();
    double d = current_state->checkLocation();//could be a better way to lay this out
    if(d<0&&wm.getNumberOfChunks()==0)//this is to quickly teach it to keep a chunk in memory
        return -100.;
    if(d>0)
    {
        // cout<<"REWARD UPPER!?!\n";
    }
    return d;
}


double lowerRewardFunction(WorkingMemory& wm)
{
    state *current_state = (state*) wm.getStateDataStructure();
    Goal g = current_state->getCurrentGoal();

    if(wm.getNumberOfChunks()==0)
    {
        // step--;
        // continue;
        return -100.;
    }
    else
    {
        Chunk direction_chunk = wm.getChunk(0);
        if(direction_chunk.getType()=="MOVE")
        {
            Direction* d = (Direction*)direction_chunk.getData();
            switch(*d)
            {
                case UP:
                current_state->step();
                current_state->hitWall = !current_state->moveUp();
                break;
                case DOWN:
                current_state->step();
                current_state->hitWall = !current_state->moveDown();
                break;
                case LEFT:
                current_state->step();
                current_state->hitWall = !current_state->moveLeft();
                break;
                case RIGHT:
                current_state->step();
                current_state->hitWall = !current_state->moveRight();
                break;
            }
        }
    }

    // if(wm.getNumberOfChunks()==0)
    // {
    //     // cout<<"NO CHUNKS!\n";
    //     return -100.;//teach it quickly to keep a move chunk in memory
    // }
    if(current_state->getAgentX()==g.x && current_state->getAgentY()==g.y && !current_state->goalReached())
    {
        current_state->atGoal();
        // cout<<"REACHD GOAL!!! at: "<<current_state->getSteps()<<"\n";
        current_state->resetSteps();
        return 50.;
    }
    if(current_state->hitWall)
    {
        // cout<<"HIT WALL*\n";
        return -5.;
    }
    return -1.;
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
 * 
 * currently the representation is and x choord y choord, concat onto their end
 * This may need to change to a 2d array though this would be really inefficent
 */
void upperStateFunction(FeatureVector& fv, WorkingMemory& wm)
{
    fv.clearVector();
    state *current_state =(state*) wm.getStateDataStructure();
    int x = current_state->getAgentX();
    int y = current_state->getAgentY()+totalSize;
    fv.setValue(x,1.);
    fv.setValue(y,1.);
    distanceClear c = current_state->getDistanceClear(0);
    
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

void lowerStateFunction(FeatureVector& fv, WorkingMemory& wm)
{
    fv.clearVector();
    state *current_state =(state*) wm.getStateDataStructure();
    int x = current_state->getAgentX();
    int y = current_state->getAgentY()+totalSize;//total size should be the offset needed
    fv.setValue(x,1.);
    fv.setValue(y,1.);
    distanceClear c = current_state->getDistanceClear(0);
    
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
    //end identical part
    

    x = (2*totalSize) + current_state->getCurrentGoal().x;
    y = (3*totalSize) + current_state->getCurrentGoal().y;
    fv.setValue(x,1.);
    fv.setValue(y,1.);
    c = current_state->getDistanceClear(1);
    
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

void upperChunkFunction(FeatureVector& fv, Chunk& chk, WorkingMemory& wm)
{
    fv.clearVector();
    if(chk.getType()=="GOAL")
    {
        Goal* g = (Goal*)chk.getData();
        fv.setValue(g->x,1.);
        fv.setValue(g->y+totalSize, 1.);
    }
}

void lowerChunkFunction(FeatureVector& fv, Chunk& chk, WorkingMemory& wm)
{
    fv.clearVector();
    if(chk.getType()=="MOVE")
    {
        Direction* dir = (Direction*) chk.getData();
        switch(*dir)
        {
            case UP:
            fv.setValue(0,1.);
            break;
            case DOWN:
            fv.setValue(1,1.);
            break;
            case LEFT:
            fv.setValue(2,1.);
            break;
            case RIGHT:
            fv.setValue(3,1.);
            break;
        }
    }
}

void deleteChunkFunction(Chunk& chk)
{
    if(chk.getType()=="MOVE")
    {
        delete ((Direction*) chk.getData());
        chk.setType("EMPTY");
    }
    else if(chk.getType()=="GOAL")
    {
        delete ((Goal*) chk.getData());
        chk.setType("EMPTY");
    }
    else{
        cerr<<"**ERROR** Unidentified chunk attempting to be deleted!\n";
    }
}




