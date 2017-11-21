

#ifndef Level_h
#define Level_h

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#undef main
/* Contains the background as well as any static foreground elements
 * that need to move independently from the camera.
 */
class Level{
public:
    
    ~Level();
    /******
     * TERRAIN MANAGEMENT
     ******/
    void addTerrain(int x, int y, int w, int h);
    
    void addTerrainRect(SDL_Rect *rect);
    
	/*******
     * TEXTURE RENDERING AND LOADING
     *******/
	void renderBG(int x, int y, SDL_Rect* camera, SDL_Renderer* renderer);
    
    //Sets the active background texture to the element at extraTextures[index].
	void setBG(int index);
    
    /* Adds a texture to the extraTextures vector. The new texture can be swapped
     * in as the new active texture using the setBG function.
     */
    void addTexture(SDL_Texture* newTex);
                    
    /*******
     * GETTERS
     *******/
                    
	//getTerrain() returns reference for camera operation in GameEngine.cpp
    std::vector<SDL_Rect>& getTerrain();
    
    size_t numWalls();
    
private:
    
    //Active texture to render as the level background
    SDL_Texture* background;
    
    //Vector of additional background textures that can be switched in as the active texture.
    std::vector<SDL_Texture*> extraTextures;
    
    //Vector containing all walls and platforms in the level
    std::vector<SDL_Rect> terrain;	
};

#endif /* Level_h */

