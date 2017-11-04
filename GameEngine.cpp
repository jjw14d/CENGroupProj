

#include "GameEngine.h"

GameEngine::GameEngine(){
    
    //Initialize member data
    
    running = true;
    winW = 800;
    winH = 1100;
    
    //add platforms for the player to stand on
    level.addTerrain(0, 400, 400, 20);
    level.addTerrain(400, 450, 300, 20);
    
    //initialize SDL video and event subsystems
    
    SDL_Init(SDL_INIT_VIDEO);
    
    //initialize window with specified name and settings
    
    window = SDL_CreateWindow("projectS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, SDL_WINDOW_SHOWN);
    
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
     * a quit event. Keyboard input will be handled
     * later.
     */
    
    //true only if the player moves
    bool motion;
    
    //initial render
    SDL_RenderClear(renderer);
    
    player.animate(4, renderer);
    
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
    
        /* Here we handle keyboard input. First, we initialize an array containing the current state
         * of every key on the keyboard. Then, we check the states of they keys we care
         * about and take action accordingly.
         */
        
         const Uint8* keyStates = SDL_GetKeyboardState( NULL );
        
        if( keyStates[ SDL_SCANCODE_UP ] )
        {
            //We only want to jump if the player is actually sitting on something
            for (int i = 0; i < level.numWalls(); i++){
                if (isOnTop(*player.getPos(), level.getTerrain()[i])){
                    player.jump();
                    break;
                }
            }
        }
        
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

        //update player position
        player.move();
        
        /* Here we check for collision after adjusting entity positions.
         * if collisions are detected, we move the player to the top of the
         * SDL_Rect with which it is colliding.
         */
        
        for (int i = 0; i < level.numWalls(); i++)
        if (checkCollision(level.getTerrain()[i], *player.getPos()))
            player.setYPos(level.getTerrain()[i].y - player.getHeight());
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

        SDL_RenderClear(renderer);
        
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        for (int i = 0; i < level.numWalls(); i++)
            SDL_RenderDrawRect(renderer, &level.getTerrain()[i]);
        
        //animate only if there was horizontal movement. Otherwise, just re-render.
        if (motion)
            player.animate(4, renderer);
        else
            player.render(renderer);

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
