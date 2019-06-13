#include <iostream>
#include "FourRoom.h"

using namespace std;

//this really just calls the main function in FourRoom, but it allows for passing of some variables to determine logging, and never ending
int main(int argc, char* argv[])
{

    //This block sets up the options for debugging, and ignoring the sliding window END condition
    char val = '0';
    if(argc>1)
        val = *argv[1];
    bool debug = val=='1'||val=='3';
    bool dontEnd = val=='2'||val=='3';
    

    cout<<"Starting Simulation:\n";
    RunSimulation(debug,dontEnd);
    return 0;
}