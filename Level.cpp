

#include "Level.h"

void Level::addTerrain(int x, int y, int w, int h){
    SDL_Rect newTerrain;
    newTerrain.x = x;
    newTerrain.y = y;
    newTerrain.w = w;
    newTerrain.h = h;
    
    terrain.push_back(newTerrain);
}

//getters

std::vector<SDL_Rect>& Level::getTerrain(){
    return terrain;
}

size_t Level::numWalls(){
    return terrain.size();
}

void Level::renderBG(int x, int y, SDL_Rect* camera, SDL_Renderer* renderer) {
	//only portion of background in camera's view will be rendered
	SDL_Rect renderSpace = { x, y, 0, 0 };
	renderSpace.w = camera->w;
	renderSpace.h = camera->h;
	SDL_RenderCopy(renderer, background, camera, &renderSpace);
}


void Level::setBG(SDL_Texture* bgTex) {
	background = bgTex;
}


