

#include "GameEngine.h"

//globals for screen and level size
const int LEVEL_WIDTH = 1080;
const int LEVEL_HEIGHT = 480;

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

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
    
	//load background image
	level.setBG(loadGraphics("bg.png"));
    
    // create a few enemies
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
    walkers.back().setPosRect(100, 100, 800, 300);
    walkers.back().setSpriteSheet(loadGraphics("somersault.png"));

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, player.getTexture(), NULL,NULL);
    SDL_RenderPresent(renderer);
}

GameEngine::~GameEngine(){
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
}

void GameEngine::run(){
    
    /* The SDL_Event object will be used only to poll for
     * a quit event, and to handle player jumping . Keyboard input will be handled
     * later.
     */
    
    //true only if the player moves
    bool motion;
    
	//camera area, initialized
	SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	
    //initial render
    SDL_RenderClear(renderer);
	
	
	
    player.animate(4, renderer, camera.x, camera.y);
	
       
    SDL_RenderPresent(renderer);
    
    SDL_Event event;
    while (running){
        motion = false;
        SDL_PollEvent(&event);
    
		switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			default:
				break;
		}
		//jumping handled as event to avoid player hopping continuously when UP held down
		if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
				switch (event.key.keysym.sym) {
				case SDLK_UP:
					//We only want to jump if the player is actually sitting on something
					for (int i = 0; i < level.numWalls(); i++) {
						if (isOnTop(*player.getPos(), level.getTerrain()[i])) {
							player.jump();
							break;
						}
					}
				default:
					break;
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
			//terrain vector is returned as reference so offset can ve calculated
			level.getTerrain()[i].x = level.getTerrain()[i].x - camera.x;
			level.getTerrain()[i].y = level.getTerrain()[i].y - camera.y;
			
			SDL_RenderDrawRect(renderer, &level.getTerrain()[i]);
			
			//add offset back to restore original coordinate values
			level.getTerrain()[i].x = level.getTerrain()[i].x + camera.x;
			level.getTerrain()[i].y = level.getTerrain()[i].y + camera.y;
			
		}
		
        for (auto itr = walkers.begin(); itr != walkers.end(); ++itr)
        {
            if (checkCollision(*itr->getPos(), *player.getPos()))
            {
                walkers.erase(itr);
                break;  // don't collide with 2 walkers at once
                        // also iterators can be weird when you erase them
            }
        }
        for (auto itr = walkers.begin(); itr != walkers.end(); ++itr)
        {
            itr->Activity();
        }
        for (int i = 0; i < walkers.size(); ++i)
        {
            walkers[i].animate(3, renderer, camera.x, camera.y);
        }
		
        //animate only if there was horizontal movement. Otherwise, just re-render.
        if (motion)
            player.animate(3, renderer, camera.x, camera.y);
        else
            player.render(renderer, camera.x, camera.y);
		
		
        SDL_RenderPresent(renderer);
       
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
