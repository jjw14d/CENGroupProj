

#ifndef Level_h
#define Level_h

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

/* Contains the background as well as any static foreground elements
 * that need to move independently from the camera.
 */
class Level{
public:
    
    //add a wall or platform to the level
    void addTerrain(int x, int y, int w, int h);
    
    void addTerrainRect(SDL_Rect *rect);
    
    //getters
    std::vector<SDL_Rect> getTerrain();
    
    size_t numWalls();
    
private:
    //A texture to render as the level background
    SDL_Texture* background;
    
    //Vector containing all walls and platforms in the level
    std::vector<SDL_Rect> terrain;
};

#endif /* Level_h */

