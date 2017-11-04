
#ifndef GameEngine_h
#define GameEngine_h

#include <stdio.h>
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Entity.h"
#include "Level.h"

/* This class is in charge of initializing and managing
 * all in-game elements as well as starting the game loop.
 */

class GameEngine{
public:
    GameEngine();
    ~GameEngine();
    
    //Initializes the SDL_Window and Renderer for the game
    void init_SDL();
    
    //Runs the main game loop. This will run until the game exits.
    void run();
    
    /******* UTILITY FUNCTIONS ********
     **********************************/
    
    //checks for collision between two SDL_Rect objects
    bool checkCollision(SDL_Rect box1, SDL_Rect box2);
    
    
    //checks whether box1 is sitting on top of surface
    bool isOnTop(SDL_Rect box1, SDL_Rect surface);
    
    //Loads image at the specified path and returns it as an SDL_Texture object
    SDL_Texture* loadGraphics(std::string path);
    
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    //Player object
    Player player;
    
    //Contains background graphics as well as hitbox data for the level
    Level level;
    
    //Monster vector
    std::vector<Entity> monsters;
    
    //window dimensions
    int winW;
    int winH;
    
    //flag for the game loop
    bool running;
};

 
#endif /* GameEngine_hpp */
