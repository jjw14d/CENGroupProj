

#include <iostream>
#include "GameEngine.h"
#include<SDL.h>
#undef main
int main(int argc, const char * argv[]) {

	/*
	*If player reaches the end of the level and selects "NEXT", run() returns true.
	*Currently, only one level is supported, therefore selecting "NEXT" only
	*restarts the level from the beginning. The old GameEngine is implicitly 
	*destroyed and a new one is created. A GameEngine object conatins hard-coded
	*information about the level/player, so it must be recreated before every new run.
	*If more levels are implemented in the future, it will be easy to cycle through them
	*using a method similar to this one.
	*/
	bool nextLevel = true;
	
	
	while(nextLevel == 1)
	{
		GameEngine g;
		nextLevel = g.run();
	}
		
		

    return 0;
}
