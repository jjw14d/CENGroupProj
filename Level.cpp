

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

std::vector<SDL_Rect> Level::getTerrain(){
    return terrain;
}

size_t Level::numWalls(){
    return terrain.size();
}
