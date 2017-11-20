

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
    
    //create some monsters
    walkers.push_back(PlatformWalker());
    walkers.back().setPlatform(level.getTerrain()[1]);
    walkers.back().setPosRect(100, 100, 400, 350);
    walkers.back().setSpriteSheet(loadGraphics("somersault.png"));

    walkers.push_back(PlatformWalker());
    walkers.back().setPlatform(level.getTerrain()[1]);
    walkers.back().setPosRect(100, 100, 600, 350);
    walkers.back().setSpriteSheet(loadGraphics("somersault.png"));

    walkers.push_back(PlatformWalker());
    walkers.back().setPlatform(level.getTerrain()[3]);
    walkers.back().setSpriteSheet(loadGraphics("somersault.png"));
    walkers.back().setPosRect(100, 100, 800, 300);

	//Load background images for platform and battle segments
	level.addTexture(loadGraphics("BG.png"));
    level.addTexture(loadGraphics("BattleBG.png"));
    
    //Load menu textures
    menuTex.push_back(loadGraphics("battleMenuMain.png"));
    menuTex.push_back(loadGraphics("inventoryMenuMain.png"));
      
    //Set the background texture to the platforming background intially.
    level.setBG(0);
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, player.getTexture(), NULL,NULL);
    SDL_RenderPresent(renderer);
}

GameEngine::~GameEngine(){
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    for (int i = 0; i < menuTex.size(); i++){
        SDL_DestroyTexture(menuTex[i]);
    }

    walkers.clear();
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
        
            /* Check for collision between player and monster. If there is a collision,
             * set gamemode to BATTLE and set the battleMonsterIndex to the index of the
             * monster the player is in collision with.
             */
        
            for (int i = 0; i < walkers.size(); i++){
                if (checkCollision(*player.getPos(), *walkers[i].getPos())){
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
         
            for (int i = 0; i < walkers.size(); i++){
                // Animate renders relative to camera
                walkers[i].Activity();
                walkers[i].animate(3, renderer, camera.x, camera.y);

                /* must be rendered relative to the camera, so we subtract the camera's
                * position.
                */
            /*    
                walkers[i].setXPos(walkers[i].getxPos() - camera.x);
                walkers[i].setYPos(walkers[i].getyPos() - camera.y);
            
                SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

                SDL_RenderDrawRect(renderer, walkers[i].getPos());
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                walkers[i].setXPos(walkers[i].getxPos() + camera.x);
                walkers[i].setYPos(walkers[i].getyPos() + camera.y);
           */

               
                
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
                
                walkers[battleMonsterIndex].setXPos(600);
                walkers[battleMonsterIndex].setYPos(200);
                
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
                                    //Fight: subtract 1 from the monster's HP. Exit if == 0.
                                    walkers[battleMonsterIndex].decrementHitPoints(1);
                                    if (walkers[battleMonsterIndex].getHP() == 0)
                                        gameMode = PLATFORM;
                                    break;
                                case 402:
                                    //Run: if the user flees the battle, return to the platform segment.
                                    gameMode = PLATFORM;
                                default:
                                    break;
                            }
                            break;
                    }
                }
            }

            //update health bars
            playerHealthBar.w = 10 * player.getHP();
            monsterHealthBar.w = 10 * walkers[battleMonsterIndex].getHP();

            //re-render
            SDL_RenderClear(renderer);
            
            level.renderBG(0, 0, &staticCam, renderer);
            
            SDL_RenderCopy(renderer, menuTex[0], NULL, &staticCam);
            
            player.animate(4, renderer, staticCam.x, staticCam.y);
            
            //SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
            //SDL_RenderDrawRect(renderer, monsters[battleMonsterIndex].getPos());
            
            walkers[battleMonsterIndex].animate(4, renderer, staticCam.x, staticCam.y);
            
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
                walkers.erase(walkers.begin() + battleMonsterIndex);
                
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
