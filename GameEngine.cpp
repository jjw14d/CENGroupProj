

#include "GameEngine.h"

//globals for screen and level size
const int LEVEL_WIDTH = 1080;
const int LEVEL_HEIGHT = 480;

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

enum gameMode {PLATFORM, BATTLE, INVENTORY};

GameEngine::GameEngine(){
    
    //Initialize member data
    
    running = true;
	//winW = 800;
	//winH = 1100;
	
    //add platforms for the player to stand on
    level.addTerrain(0, 400, 350, 20);
    level.addTerrain(400, 450, 300, 20);
	level.addTerrain(450, 300, 300, 20);
	level.addTerrain(800, 400, 300, 20);
    
    //create some monsters
    Monster* mon1 = new Monster(); //I set the position rectangles later on when it happens with the player entity
    //mon1->setYPos(200);
    //mon1->setXPos(500);
    Monster* mon2 = new Monster();
    //mon2->setYPos(200);
    //mon2->setXPos(800);

    monsters.push_back(*mon1);
    monsters.push_back(*mon2);
    //delete mon1;
	
    
    //initialize SDL video and event subsystems
    
    SDL_Init(SDL_INIT_VIDEO);
    
    //initialize window with specified name and settings
    
    window = SDL_CreateWindow("projectS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    
    /* initalize renderer on the specified window with the specified flags.
     * In this case, the renderer is set to use hardware acceleration, and
     * the render rate will be synchronized with the display's refresh rate.
     */
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (renderer == nullptr) {
        std::cout << "Renderer could not be created" << std::endl;
    }
    else {
        SDL_SetRenderDrawColor(renderer, 1, 0, 1, 1);
        
        int imgFlags = IMG_INIT_PNG;
        
        if ( !(IMG_Init(imgFlags) & imgFlags)) {
            std::cout << "SDL_image could not initialize\n" << IMG_GetError;
        }
    }
    
    player.setSpriteSheet(loadGraphics("somersault.png"));
    player.setPosRect(100, 100);
    
	//Load background images for platform and battle segments
	level.addTexture(loadGraphics("BG.png"));
    level.addTexture(loadGraphics("BattleBG.png"));
	
	//load enemy sprites and postition rectangles, just like with player
	mon1->setSpriteSheet(loadGraphics("somersault_red.png")); //I just recolored the protagonist sprite for now
	mon1->setPosRect(100, 100, 300, 200); //put on the third platform
	mon2->setSpriteSheet(loadGraphics("somersault_red.png"));
	mon2->setPosRect(100, 100, 700, 200); //put this guy on the last platform
    
    //Load menu textures
    menuTex.push_back(loadGraphics("battleMenuMain.png"));
    menuTex.push_back(loadGraphics("inventoryMenuMain.png"));
    
    //Set the background texture to the platforming background intially.
    level.setBG(0);
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, player.getTexture(), NULL,NULL);
    SDL_RenderCopy(renderer, mon1->getTexture(), NULL, NULL);
    SDL_RenderCopy(renderer, mon2->getTexture(), NULL, NULL); 
    SDL_RenderPresent(renderer);
}

GameEngine::~GameEngine(){
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    for (int i = 0; i < menuTex.size(); i++){
        SDL_DestroyTexture(menuTex[i]);
    }
}

void GameEngine::run(){
    
    /* The SDL_Event object will be used only to poll for
     * a quit event, and to handle player jumping . Keyboard input will be handled
     * later.
     */
    
    //Integer flag to indicate whether we are in battle mode or platform mode.
    int gameMode = PLATFORM;
    
    //true only if the player moves
    bool motion;
	bool mon1Motion;
	bool mon2Motion; 
    
    //Element of the monster to take into the battle segment
    int battleMonsterIndex;
    
    //Small rectangle to use as a menu cursor.
    SDL_Rect menuCursor = { 0, 0, 10, 10 };
    
    //Camera frame used to scroll across the level.
	SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    
    /* Dummy camera for rendering purposes. Currently,
     * the camera does not move in battle mode.
     */
    SDL_Rect staticCam = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    


    //initial render
    SDL_RenderClear(renderer);

    player.animate(4, renderer, camera.x, camera.y);
	
	mon1->animate(4, renderer, camera.x, camera.y);
	mon2->animate(4, renderer, camera.x, camera.y); //animate enemy sprites just like a player

    SDL_RenderPresent(renderer);
    
    SDL_Event event;
    
    while (running){
      
        /***********************
         * PLATFORMING SEGMENT *
         ***********************/
        
        if (gameMode == PLATFORM){
            motion = false;
		mon1Motion = false;
		mon2Motion = false;
            //SDL_PollEvent(&event);
        
            while (SDL_PollEvent(&event)){
                switch (event.type) {
                    case SDL_QUIT:
                        running = false;
                        break;
                    default:
                        break;
                }
                
                if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
                    //jumping handled as event to avoid player hopping continuously when UP held down
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            //We only want to jump if the player is actually sitting on something
                            for (int i = 0; i < level.numWalls(); i++) {
                                if (isOnTop(*player.getPos(), level.getTerrain()[i])) {
                                    player.jump();
                                    break;
                                }
                            }
                            break;
                        case SDLK_RETURN:
                            if (event.key.repeat == 0)
                                gameMode = INVENTORY;
                                break;
                        default:
                            break;
                    }
                }
            }
        
            /* Here we handle keyboard input. First, we initialize an array containing the current state
             * of every key on the keyboard. Then, we check the states of they keys we care
             * about and take action accordingly.
             */
        
            const Uint8* keyStates = SDL_GetKeyboardState( NULL );
            
            if( keyStates[ SDL_SCANCODE_DOWN ] )
            {
                
            }
        
            if( keyStates[ SDL_SCANCODE_LEFT ] )
            {
                //face left
                player.switchAnimationChannel(1);
                
                player.goLeft();
                motion = true;
            }
            else if( keyStates[ SDL_SCANCODE_RIGHT ] )
            {
                //face right
                player.switchAnimationChannel(0);
                
                player.goRight();
                motion = true;
            }
            /* if neither right or left keys are pressed, we stop the player object's
             * horizontal movement.
             */
            else
                player.stopX();
        
            //apply gravity
            player.fall();
		
		//setting the enemy movement, using functions from entity.h. Should make him just patrol back on forth on whatever platform he spawns on
		for (unsigned int i = 0; i < level.numWalls(); i++) //check collision, make sure the enemy is actually standing on something to patrol
		{
			if (isOnTop(*mon1->getPos(), level.getTerrain()[i]))
			{
				SDL_Rect currentPlatform = level.getTerrain()[i]; //the rectangle the enemy is currently standing on
				if ((mon1->getxPos() + mon1->getXVel()) < currentPlatform.x) //if the enemy is about to hit the left side of the platform
				{
					mon1->stopX(); //stop motion
					mon1->switchAnimationChannel(0); //face right
					mon1->goRight();
					mon1Motion = true;
				}
				else if ((mon1->getxPos() + mon1->getXVel()) > (currentPlatform.x + currentPlatform.w)) // if the enemy is about to hit the right side of the platform
				{
					mon1->stopX();
					mon1->switchAnimationChannel(1); //face left
					mon1->goLeft();
					mon1Motion = true;
				}
				else if ((mon1->getxPos() > currentPlatform.x) && (mon1->getxPos() < currentPlatform.x + currentPlatform.w))
				{
					if (mon1->getXVel() == 0) //if in the middle of the platform and not moving (like right after it spawns)
					{
						mon1->goRight();
						mon1Motion = true;
					}
				}
			}
		}
		
		//do the same for the second monster, but he starts going left
		for (unsigned int i = 0; i < level.numWalls(); i++) //check collision, make sure the enemy is actually standing on something to patrol
		{
			if (isOnTop(*mon2->getPos(), level.getTerrain()[i]))
			{
				SDL_Rect currentPlatform = level.getTerrain()[i]; //the rectangle the enemy is currently standing on
				if ((mon2->getxPos() + mon2->getXVel()) < currentPlatform.x) //if the enemy is about to hit the left side of the platform
				{
					mon2->stopX(); //stop motion
					mon2->switchAnimationChannel(0); //face right
					mon2->goRight();
					mon2Motion = true;
				}
				else if ((mon2->getxPos() + mon2->getXVel()) > (currentPlatform.x + currentPlatform.w)) // if the enemy is about to hit the right side of the platform
				{
					mon2->stopX();
					mon2->switchAnimationChannel(1); //face left
					mon2->goLeft();
					mon2Motion = true;
				}
				else if ((mon2->getxPos() > currentPlatform.x) && (mon2->getxPos() < currentPlatform.x + currentPlatform.w))
				{
					if (mon2->getXVel() == 0) //if in the middle of the platform and not moving (like right after it spawns)
					{
						mon2->goLeft();
						mon2Motion = true;
					}
				}
			}
		}

            /* Here we update the player position (previously done
            * with the move() function) and check for collision.
            * Upon collision, the players position is reverted
            * back to prohibit further movement in the x or y
            * directions.
            */
		
            player.setXPos(player.getxPos() + player.getXVel());
            for (int i = 0; i < level.numWalls(); i++)
                if (checkCollision(level.getTerrain()[i], *player.getPos()))
                    player.setXPos(player.getxPos() - player.getXVel());
        
            player.setYPos(player.getyPos() + player.getYVel());
            for (int i = 0; i < level.numWalls(); i++)
                if (checkCollision(level.getTerrain()[i], *player.getPos()))
                {
                    player.setYPos(player.getyPos() - player.getYVel());
                    /* Reverting the players y postion after collision
                    * will occasionally result in a y postion slightly lower
                    * than the platform on which the player stands. This
                    * makes the isOnTop() function return false and breaks
                    * the ability to jump. This problem is "fixed" with
                    * the following if-statement.
                    */
                    if(player.getyPos() < level.getTerrain()[i].y)
                        player.setYPos(level.getTerrain()[i].y - player.getHeight());
                }
		
		//update enemy position just like a moving player
		mon1->setXPos(mon1->getxPos() + mon1->getXVel());
		for (int i = 0; i < level.numWalls(); i++) //if he bumps into anything, I also want to set him walking the other way
		{
			if (checkCollision(level.getTerrain()[i], mon1->.getPos()))
			{
				mon1->setXPos(mon1->getxPos() - mon1->getXVel());
				if (mon1->getXVel() > 0) //if he's heading right
				{
					mon1->stopX();
					mon1->goLeft();
				}
				else if (mon1->getXVel() < 0)
				{
					mon1->stopX();
					mon1->goRight();
				}
			}
		}
		
		//do the same for the second enemy
		for (int i = 0; i < level.numWalls(); i++) //if he bumps into anything, I also want to set him walking the other way
		{
			if (checkCollision(level.getTerrain()[i], mon2->.getPos()))
			{
				mon1->setXPos(mon2->getxPos() - mon2->getXVel());
				if (mon2->getXVel() > 0) //if he's heading right
				{
					mon2->stopX();
					mon2->goLeft();
				}
				else if (mon2->getXVel() < 0)
				{
					mon2->stopX();
					mon2->goRight();
				}
			}
		}
        
            /* Check for collision between player and monster. If there is a collision,
             * set gamemode to BATTLE and set the battleMonsterIndex to the index of the
             * monster the player is in collision with.
             */
        
            for (int i = 0; i < monsters.size(); i++){
                if (checkCollision(*player.getPos(), *monsters[i].getPos())){
                    gameMode = BATTLE;
                    battleMonsterIndex = i;
                }
            }
        
            //center camera over the player
            camera.x = (player.getxPos() + player.getWidth() / 2) - SCREEN_WIDTH / 2;
            camera.y = (player.getyPos() + player.getHeight() / 2) - SCREEN_HEIGHT / 2;
        
            //Keep the camera in bounds
            if (camera.x < 0)
            {
                camera.x = 0;
            }
            if (camera.y < 0)
            {
                camera.y = 0;
            }
            if (camera.x > LEVEL_WIDTH - camera.w)
            {
                camera.x = LEVEL_WIDTH - camera.w;
            }
            if (camera.y > LEVEL_HEIGHT - camera.h)
            {
                camera.y = LEVEL_HEIGHT - camera.h;
            }
        
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

            SDL_RenderClear(renderer);
        
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(renderer);

            level.renderBG(0, 0, &camera, renderer);
        
            //render walls and platforms
            for (int i = 0; i < level.numWalls(); i++) {
                //must be rendered relative to the camera, so subtracting camera position
                //terrain vector is returned as reference so offset can be calculated
                level.getTerrain()[i].x = level.getTerrain()[i].x - camera.x;
                level.getTerrain()[i].y = level.getTerrain()[i].y - camera.y;
                
                SDL_RenderDrawRect(renderer, &level.getTerrain()[i]);
                
                //add offset back to restore original coordinate values
                level.getTerrain()[i].x = level.getTerrain()[i].x + camera.x;
                level.getTerrain()[i].y = level.getTerrain()[i].y + camera.y;
                
            }
        
            //render monsters
           // I just used entity functions to render monsters
    
            //animate only if there was horizontal movement. Otherwise, just re-render.
            if (motion)
                player.animate(3, renderer, camera.x, camera.y);
            else
                player.render(renderer, camera.x, camera.y);
		
		//animate monsters if in motion
		if(mon1Motion == true)
			mon1->animate(3, renderer, camera.x, camera.y);
		else
			mon1->render(renderer, camera.x, camera.y);
		if(mon2Motion == true)
			mon2->animate(3, renderer, camera.x, camera.y);
		else
			mon2->render(renderer, camera.x, camera.y);
        
            SDL_RenderPresent(renderer);
            
            //Prepare to enter battle mode
            if (gameMode == BATTLE){
                player.savePosition();
                player.setXPos(100);
                player.setYPos(200);
                
                monsters[battleMonsterIndex].setXPos(600);
                monsters[battleMonsterIndex].setYPos(200);
                
                //Place the menu cursor on the battle menu
                menuCursor.x = 100;
                menuCursor.y = 372;
                
                //switch active background texture to the battle background
                level.setBG(1);
            }
         }
        
        
        /******************
         * BATTLE SEGMENT *
         ******************/
        else if (gameMode == BATTLE){
		
	//Rectangles for monster and player health bars. I just have them scale with total hitpoints right now
    	SDL_Rect playerHealthBar = {100, 100, player.getHealth(), 10};
    	SDL_Rect monsterHealthBar = {570, 100, monsters[battleMonsterIndex].getHealth, 10};
            
            while( SDL_PollEvent( &event ) != 0 )
            {
                //Check whether the user is trying to quit.
                if( event.type == SDL_QUIT )
                {
                    running = false;
                }
                
                else if( event.type == SDL_KEYDOWN )
                {
                    
                    switch( event.key.keysym.sym )
                    {
                        case SDLK_UP:
                            menuCursor.y -= 15;
                            if (menuCursor.y < 372)
                                menuCursor.y = 402;
                            break;
                            
                        case SDLK_DOWN:
                            menuCursor.y += 15;
                            if (menuCursor.y > 402)
                                menuCursor.y = 372;
                            break;
                        case SDLK_RETURN:
                            //determine the desired menu option based on the cursor's position
                            switch (menuCursor.y) {
                            
				case 372:	  
                                    monsters[battleMonsterIndex].getHit(player.attack()); //just rewrote with the new functions
                                    if (monsters[battleMonsterIndex].getHealth() <= 0)
				    {
					player.addExp(monsters[battleMonsterIndex].getExpVal());
					if(player.checkLevelUp())
						player.levelUp(); //trigger level up automatically after experience is added
                                        gameMode = PLATFORM;
				    }
                                    break;
                                case 402:
                                    //Run: if the user flees the battle, return to the platform segment.
                                    gameMode = PLATFORM;
				    //if they flee, only give the player a portion of the experience
				    player.addExp((monsters[battleMonsterIndex].getExpVal()) / 10);
                                default:
                                    break;
                            }
                            break;
                    }
                }
            }

            //update health bars
	    if(player.getHealth() < 0)	
            	playerHealthBar.w = 0; //to prevent a negative width
	    else
		playerHealthBar.w = player.getHealth();     
	    if(monsters[battleMonsterIndex].getHealth() < 0)
            	monsterHealthBar.w = 0;
	    else
		monsterHealthBar.w = monsters[battleMonsterIndex].getHealth();

            //re-render
            SDL_RenderClear(renderer);
            
            level.renderBG(0, 0, &staticCam, renderer);
            
            SDL_RenderCopy(renderer, menuTex[0], NULL, &staticCam);
            
            player.animate(4, renderer, staticCam.x, staticCam.y);
		
	    monsters[battleMonsterIndex].animate(4, renderer, staticCam.x, staticCam.y);	
            
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
            //SDL_RenderDrawRect(renderer, monsters[battleMonsterIndex].getPos()); Shouldn't need this anymore I think?
            
            //draw health bars
            SDL_RenderFillRect(renderer, &playerHealthBar);
            SDL_RenderFillRect(renderer, &monsterHealthBar);
            
            //draw the cursor
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
            SDL_RenderDrawRect(renderer, &menuCursor);
            
            

            SDL_RenderPresent(renderer);
            
            //Prepare to return to platform mode.
            if (gameMode == PLATFORM){
                //load the player's old position
                player.loadPosition();
                
                //destroy the monster we were fighting
                monsters.erase(monsters.begin() + battleMonsterIndex);
                
                //switch back to the platform segment's background texture.
                level.setBG(0);
            }
    
        }
        /*********************
         * INVENTORY SEGMENT *
         *********************/
        else if (gameMode == INVENTORY){
            while( SDL_PollEvent( &event ) != 0 )
            {
                //Check whether the user is trying to quit.
                if( event.type == SDL_QUIT )
                {
                    running = false;
                }
                
                else if( event.type == SDL_KEYDOWN )
                {
                    switch( event.key.keysym.sym )
                    {
                        case SDLK_UP:
                            break;
                        case SDLK_DOWN:
                            break;
                        case SDLK_RETURN:
                            gameMode = PLATFORM;
                            break;
                        default:
                            break;
                    }
                }
            }
            
            //re-render
            SDL_RenderCopy(renderer, menuTex[1], NULL, &staticCam);
            
            SDL_RenderPresent(renderer);
            
        }
    }
}

//Checks for collsion between two SDL_Rect objects
bool GameEngine::checkCollision(SDL_Rect box1, SDL_Rect box2){
    int topA, topB;
    int bottomA, bottomB;
    int leftA, leftB;
    int rightA, rightB;
    
    //find where each side of each hitbox lies on its axis
    
    topA = box1.y;
    bottomA = box1.y + box1.h;
    leftA = box1.x;
    rightA = box1.x + box1.w;
    
    topB = box2.y;
    bottomB = box2.y + box2.h;
    leftB = box2.x;
    rightB = box2.x + box2.w;
    
    
    if (leftA >= rightB || leftB >= rightA){
        return false;
    }
    if (bottomB <= topA || bottomA <= topB){
        return false;
    }
    else
        return true;
}

bool GameEngine::isOnTop(SDL_Rect box1, SDL_Rect surface){
    
    if (box1.x > surface.x + surface.w)
        return false;
    if (box1.x + box1.w < surface.x)
        return false;
    if (box1.y + box1.h == surface.y )
        return true;
    else
        return false;
}

//Loads image at the specified path and returns it as an SDL_Texture object
SDL_Texture* GameEngine::loadGraphics(std::string path){
    
    //The texture we will return
    
    SDL_Texture* newTexture = nullptr;
    
    //Load the image to a surface in preperation for texture creation.
    
    SDL_Surface* newSurface = IMG_Load(path.c_str());
    
    //make sure the image loaded properly
    
    if (newSurface == NULL)
        std::cout << "SDL_image error: The image at " << path << " could not be loaded\n";
    
    else{
        
        /****************
         * TRANSPARENCY *
         ****************
         * By changing the RGB parameters in the SDL_MapRGB function call,
         * we can change the transparent color for sprites. Currently, it
         * is set to cyan (0, 255, 255).
         */
        
        SDL_SetColorKey( newSurface, SDL_TRUE, SDL_MapRGB(newSurface->format, 0, 255, 255 ) );
        
        newTexture = SDL_CreateTextureFromSurface(renderer, newSurface);
        
        if (newTexture == NULL)
            std::cout << "Texture could not be created from " << path << "\n";
        
        SDL_FreeSurface(newSurface);
    }
    return newTexture;
}
