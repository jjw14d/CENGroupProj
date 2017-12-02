
#ifndef GameEngine_h
#define GameEngine_h

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Entity.h"
#include "Level.h"
#include "Menu.h"
//#include "Menu.h"

/* This class is in charge of initializing and managing
 * all in-game elements as well as starting the game loop.
 */

class GameEngine{
public:
    GameEngine();
    ~GameEngine();
    
    //Initializes the SDL_Window and Renderer for the game
    void init_SDL();
    
    //intializes SDL_TTF, our font utility.
    void init_TTF();
    
    //Runs the main game loop. This will run until the game exits.
    void run();
    
    
    /******* UTILITY FUNCTIONS ********
     **********************************/
    
    //checks for collision between two SDL_Rect objects
    bool checkCollision(SDL_Rect box1, SDL_Rect box2);
    
    
    //checks whether box1 is sitting on top of surface
    bool isOnTop(SDL_Rect box1, SDL_Rect surface);
    
    //Renders the provided string using TTF an returns it as a texture.
    SDL_Texture* renderString(const char* string);
    
    /*****************
     * ASSET LOADING *
     *****************/
    
    //Loads image at the specified path and returns it as an SDL_Texture object
    SDL_Texture* loadGraphics(std::string path);
    
    //Loads font for in-game menus.
    
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    //Player object
    Player player;
    
    //Contains background graphics as well as hitbox data for the level
    Level level;
    
    //Monster vector
    std::vector<Entity> monsters;
    
    //Menu font
    TTF_Font *menuFont;
    
    //Vector of in-game menus
    std::vector<Menu> menus;
    
    //Vector of menu textures
    std::vector<SDL_Texture*> menuTex;
    //Vector of text textures to place inside the menus
    std::vector<SDL_Texture*> menuOptions;
    
    //window dimensions
    //int winW;
	//int winH;
    
    //flag for the game loop
    bool running;
};

 
#endif /* GameEngine_hpp */
