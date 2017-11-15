

#ifndef Level_h
#define Level_h

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

/* Contains the background as well as any static foreground elements
 * that need to move independently from the camera.
 */
class Level{
public:
    
    //add a wall or platform to the level
    void addTerrain(int x, int y, int w, int h);
    
    void addTerrainRect(SDL_Rect *rect);
    
	//load background image and render
	void renderBG(int x, int y, SDL_Rect* camera, SDL_Renderer* renderer);
	void setBG(SDL_Texture* bgTex);
	
    //getters
	//getTerrain() returns reference for camera operation in GameEngine.cpp
    std::vector<SDL_Rect>& getTerrain();
    
    size_t numWalls();
    
private:
    //A texture to render as the level background
    SDL_Texture* background;
    
    //Vector containing all walls and platforms in the level
    std::vector<SDL_Rect> terrain;

	
};

#endif /* Level_h */

