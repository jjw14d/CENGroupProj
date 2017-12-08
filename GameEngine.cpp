

#include "GameEngine.h"

GameEngine::GameEngine(){
    
    //Initialize member data
    running = true;
	
	
    //add platforms for the player to stand on
    level.addTerrain(0, 400, 350, 20);
    level.addTerrain(400, 450, 300, 20);
	level.addTerrain(450, 300, 300, 20);
	level.addTerrain(800, 400, 300, 20);
    
    
    
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
    
    //Initialize SDL_ttf and load font
    init_TTF();
    
    //Initialize menus with texture, position, and dimensions
    Menu* menuInventory = new Menu(loadGraphics("inventoryMenuMain.png"), 100, 150, 300, 170);
    Menu* battleMenu = new Menu(loadGraphics("inventoryMenuMain.png"), 100, 150, 100, 350);
	Menu* levelEndMenu = new Menu(loadGraphics("inventoryMenuMain.png"), 100, 150, 300, 170);
    Menu* deathMenu = new Menu(loadGraphics("inventoryMenuMain.png"), 100, 150, 300, 170);
    
    //Add inventory and battle menus
    menus.push_back(menuInventory);
    menus.push_back(battleMenu);
	menus.push_back(levelEndMenu);
    menus.push_back(deathMenu);

    //Load font from file
    menus[0]->setFont("PrStart.ttf", 20);
    menus[1]->setFont("PrStart.ttf", 20);
	menus[2]->setFont("PrStart.ttf", 20);
    menus[3]->setFont("PrStart.ttf", 20);
    
    //Inventory menu options
    menus[0]->addOption(renderer, "RESUME");
    menus[0]->addOption(renderer, "ITEM");
    menus[0]->addOption(renderer, "QUIT");
    
    //Battle menu options
    menus[1]->addOption(renderer, "FIGHT");
    menus[1]->addOption(renderer, "ITEM");
    menus[1]->addOption(renderer, "RUN");

	//End of level menu options
	menus[2]->addOption(renderer, "NEXT");
	menus[2]->addOption(renderer, "QUIT");

    //Death options
    menus[3]->addOption(renderer, "RESTART");
    menus[3]->addOption(renderer, "QUIT");
    
    player.setSpriteSheet(loadGraphics("somersault.png"));
    
    /* For now, we just hard code the entity's hitbox size to equal
     * the dimensions of an animation frame.
     */
    player.setPosRect(100, 100);
    
    //create some monsters
    Floater* mon1 = new Floater(50, 50);
    mon1->setYPos(200);
    mon1->setXPos(500);
    Floater* mon2 = new Floater(50, 50);
    mon2->setYPos(200);
    mon2->setXPos(800);
    
    mon1->setSpriteSheet(loadGraphics("Floater.png"));
    mon2->setSpriteSheet(loadGraphics("Floater.png"));
    
    monsters.push_back(mon1);
    monsters.push_back(mon2);
    
	//Load background images for platform and battle segments
	level.addTexture(loadGraphics("BG.png"));
    level.addTexture(loadGraphics("BattleBG.png"));
    
    
    //Set the background texture to the platforming background intially.
    level.setBG(0);
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, player.getTexture(), NULL,NULL);
    SDL_RenderPresent(renderer);
}

GameEngine::~GameEngine(){
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    /* Menu destructors must run before TTF_Quit() is called.
     * This is because we need TTF in order to clean up
     * TTF_Font objects.
     */
    menus.clear();
    
    for (int i = 0; i < monsters.size(); i++){
        delete monsters[i];
    }
    
    for (int i = 0; i < menus.size(); i++){
        delete menus[i];
    }
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
	
}

bool GameEngine::run(){
    
    /* The SDL_Event object will be used only to poll for
     * a quit event, and to handle player jumping. Keyboard input will be handled
     * later.
     */
	bool nextLevel = false;
    //Integer flag to indicate whether we are in battle mode or platform mode.
    int gameMode = PLATFORM;
    
    //true only if the player moves
    bool motion;
    
    //Element of the monster to take into the battle segment
    int battleMonsterIndex;
    
    //Turn indicator. Used only in battlemode.
    int turn = PLAYERTURN;
    
    //Camera frame used to scroll across the level.
	SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    
    /* Dummy camera for rendering purposes. Currently,
     * the camera does not move in battle mode.
     */
    SDL_Rect staticCam = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    
    //Rectangles for monster and player health bars.
    SDL_Rect playerHealthBar = {100, 100, 100, 10};
    SDL_Rect monsterHealthBar = {570, 100, 100, 10};

    //initial render
    SDL_RenderClear(renderer);

    player.animate(4, renderer, camera.x, camera.y);

    SDL_RenderPresent(renderer);
    
    SDL_Event event;
    
    while (running){
      
        /***********************
         * PLATFORMING SEGMENT *
         ***********************/
        
        if (gameMode == PLATFORM){
            motion = false;
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
        
            /* Here we handle keyboard input. First, we initialize an array containing the
             * current state of every key on the keyboard. Then, we check the states of
             * they keys we care about and take action accordingly.
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

            /* Here we update the player position (previously done
            * with the updatePos() function) and check for collision.
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
        
            /* Check for collision between player and monster. If there is a collision,
             * set gamemode to BATTLE and set the battleMonsterIndex to the index of the
             * monster the player is in collision with.
             */
        
            for (int i = 0; i < monsters.size(); i++){
                if (checkCollision(*player.getPos(), *monsters[i]->getPos())){
                    gameMode = BATTLE;
                    
                    //Player always goes first
                    turn = PLAYERTURN;
                    
                    battleMonsterIndex = i;
                }
            }

			//Indicates the end of the level. The player reaching a given coordinate signals the levels end.
			//Currently set to the LEVEL_WIDTH (x position of 1080)
			if (player.getxPos() >= 1080)
				gameMode = LEVEL_END;

			//Indicates player fell off platform and resets level
			if (player.getyPos() > LEVEL_HEIGHT)
				gameMode = PLAYER_DEATH;
        
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
    
            
            //move and render monsters
            for (int i = 0; i < monsters.size(); i++){
                
                monsters[i]->move();
                monsters[i]->updatePos();
                
                SDL_Rect monsterRelativePos (*monsters[i]->getPos());
                
                /* must be rendered relative to the camera, so we subtract the camera's
                 * position.
                 */
                
                monsterRelativePos.x -= camera.x;
                monsterRelativePos.y -= camera.y;
                SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

                monsters[i]->animate(4, renderer, camera.x, camera.y);
                
               // SDL_RenderDrawRect(renderer, &monsterRelativePos);
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

            }
    
            //animate only if there was horizontal movement. Otherwise, just re-render.
            if (motion)
                player.animate(3, renderer, camera.x, camera.y);
            else
                player.render(renderer, camera.x, camera.y);
        
            
            SDL_RenderPresent(renderer);
            
            //Prepare to enter battle mode
            if (gameMode == BATTLE){
                player.savePosition();
                player.setXPos(100);
                player.setYPos(200);
                
                monsters[battleMonsterIndex]->setXPos(600);
                monsters[battleMonsterIndex]->setYPos(200);
            
                
                //switch active background texture to the battle background
                level.setBG(1);
            }
         }
        
        
        /******************
         * BATTLE SEGMENT *
         ******************/
        else if (gameMode == BATTLE){
            
            if (turn == PLAYERTURN){
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
                                //Decrement the menu's cursor index
                                menus[1]->cursorDecrement();
                                break;
                                
                            case SDLK_DOWN:
                                //Increment the menu's cursor index
                                menus[1]->cursorIncrement();
                                break;
                                
                            case SDLK_RETURN:
                                //determine the desired menu option based on the cursor's position
                                switch (menus[1]->getCursorPos()) {
                                
                                    case Menu::BATTLE_FIGHT:
                                       //Fight: subtract player's attack from the monster's HP. Exit if == 0.
					if (player.attackSuccess()) //check if attack hits
					{
						monsters[battleMonsterIndex]->getHit(player.attack()); //using new functions that reflect stats
					}
                                        turn = ENEMYTURN;
					if (monsters[battleMonsterIndex]->getHP() <= 0)
					{
						gameMode = PLATFORM;
						player.addExp(monsters[battleMonsterIndex]->getExpValue());
					}
                                        break;
                                    case Menu::BATTLE_RUN:
                                        //Run: if the user flees the battle, return to the platform segment.
					player.addExp((monsters[battleMonsterIndex]->getExpValue()) / 10); //give players a small exp amount for running
                                        gameMode = PLATFORM;
                                    default:
                                        break;
                                }
                                break;
                        }
                    }
                }
            }
            else if (turn == ENEMYTURN){
                
                /* When the monster's attack animation finishes, deal damage and
                 * let the player take their turn.
                 */
                if (monsters[battleMonsterIndex]->animate(4, renderer, staticCam.x, staticCam.y) == 1){
                    if (monsters[battleMonsterIndex]->getHP() <= 15) //monster has a chance to heal itself at low hp
			{
				monsters[battleMonsterIndex]->healSelf(); 
			}
		    else
		    	{
				if (monsters[battleMonsterIndex]->attackSuccess(player))
				{
					player.getHit(monsters[battleMonsterIndex]->attack()); //now based on stats
				}
			}
                    if (player.getHP() == 0)
                        gameMode = PLAYER_DEATH;
                    turn = PLAYERTURN;
                }
            }
            
            //update health bars
            playerHealthBar.w = 2 * player.getHP();
            monsterHealthBar.w = 2 * monsters[battleMonsterIndex]->getHP(); //slightly changed to fit new functions and health values

            //re-render
            SDL_RenderClear(renderer);
            
            level.renderBG(0, 0, &staticCam, renderer);
            
            //Draw the menu with the cursor at its current index
            menus[1]->draw(renderer);
            
            player.animate(4, renderer, staticCam.x, staticCam.y);
            
            monsters[battleMonsterIndex]->render(renderer, staticCam.x, staticCam.y);
            
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
            
            
            //Draw health bars
            SDL_RenderFillRect(renderer, &playerHealthBar);
            SDL_RenderFillRect(renderer, &monsterHealthBar);
            
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
            //Initial Menu Draw
            menus[0]->draw(renderer);
            
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
                            menus[0]->cursorDecrement();
                            break;
                        case SDLK_DOWN:
                            menus[0]->cursorIncrement();
                            break;
                        case SDLK_RETURN:
                            switch (menus[0]->getCursorPos()) {
                                case Menu::PAUSE_RESUME:
                                    gameMode = PLATFORM;
                                    break;
                                case Menu::PAUSE_QUIT:
                                    running = false;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            
            //present
            SDL_RenderPresent(renderer);
        }
		/***********************
		* END OF LEVEL SEGMENT *
		************************/
		else if (gameMode == LEVEL_END)
		{
			SDL_RenderClear(renderer);
			
			//render black background
			level.renderBG(0, 0, &staticCam, renderer);

			//menu draw
			menus[2]->draw(renderer);

			//set font color for texture string 
			menus[2]->setColor(255, 0, 0);
			SDL_Rect destRect = { SCREEN_WIDTH / 4 + 60, SCREEN_HEIGHT / 4 - 25, 300, 50 };
			SDL_Texture* complete = menus[2]->renderString(renderer, "LEVEL COMPLETE!");

			//present
			SDL_RenderCopy(renderer, complete, NULL, &destRect);

			while (SDL_PollEvent(&event) != 0)
			{
				//Check whether the user is trying to quit.
				if (event.type == SDL_QUIT)
				{
					running = false;
				}

				else if (event.type == SDL_KEYDOWN)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_UP:
						menus[2]->cursorDecrement();
						break;
					case SDLK_DOWN:
						menus[2]->cursorIncrement();
						break;
					case SDLK_RETURN:
						switch (menus[2]->getCursorPos()) {
						case Menu::LEVEL_END_NEXT:
							nextLevel = true;
							return nextLevel;
							break;
						case Menu::LEVEL_END_QUIT:
							running = false;
						default:
							break;
						}
						break;
					default:
						break;


					}
				}
			}
			SDL_RenderPresent(renderer);
		}
        /***********************
        * PLAYER DEATH SEGMENT *
        ************************/
        else if (gameMode == PLAYER_DEATH)
        {
            // Set background to standard black background, in case you died in combat
            SDL_RenderClear(renderer);
            level.setBG(0);

            //render black background
            level.renderBG(0, 0, &staticCam, renderer);

            //menu draw
            menus[3]->draw(renderer);

            //set font color for texture string 
            menus[3]->setColor(255, 0, 0);
            SDL_Rect destRect = { SCREEN_WIDTH / 4 + 60, SCREEN_HEIGHT / 4 - 25, 300, 50 };
            SDL_Texture* complete = menus[3]->renderString(renderer, "YOU DIED");

            //present
            SDL_RenderCopy(renderer, complete, NULL, &destRect);

            while (SDL_PollEvent(&event) != 0)
            {
                //Check whether the user is trying to quit.
                if (event.type == SDL_QUIT)
                {
                    running = false;
                }

                else if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_UP:
                        menus[3]->cursorDecrement();
                        break;
                    case SDLK_DOWN:
                        menus[3]->cursorIncrement();
                        break;
                    case SDLK_RETURN:
                        switch (menus[3]->getCursorPos())
                        {
                        case Menu::PLAYER_DEATH_RESTART:
                            nextLevel = true;
                            return nextLevel;
                            break;
                        case Menu::PLAYER_DEATH_QUIT:
                            running = false;
                        default:
                            break;
                        }
                        break;
                    default:
                        break;


                    }
                }
            }
            SDL_RenderPresent(renderer);
        }
    }
	return nextLevel;
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

void GameEngine::init_TTF(){
    
    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        std::cout << "SDL_ttf failed to initialize:" << TTF_GetError() << "\n";
        return;
    }
}

//Loads image at the specified path and returns it as an SDL_Texture object
SDL_Texture* GameEngine::loadGraphics(std::string path){
    
    //The texture we will return
    
    SDL_Texture* newTexture = nullptr;
    
    //Load the image to a surface in preperation for texture creation.
    
    SDL_Surface* newSurface = IMG_Load(path.c_str());
    
    //Make sure the image loaded properly
    
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

SDL_Texture* GameEngine::renderString(const char* string){
    //The texture to return
    SDL_Texture* stringTexture = nullptr;
    
    SDL_Color fontColor = {255, 0, 0};
   
    stringTexture = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(menuFont, string, fontColor));
    
    if (stringTexture == nullptr){
        std::cout << "Failed to render text.\n";
        return nullptr;
    }
    
    return stringTexture;
}
