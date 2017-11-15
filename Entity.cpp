
#include "Entity.h"

/*
 **********
 ENTITY ***
 **********
 */

Entity::Entity(){
    position.x = 0;
    position.y = 0;
    position.w = 0;
    position.h = 0;
    
    animationFrame.x = 0;
    animationFrame.y = 0;
    animationFrame.w = 0;
    animationFrame.h = 0;
    spriteSheet = nullptr;
    
    xVel = 0;
    yVel = 0;
}


Entity::Entity(double h, double w, SDL_Texture* spriteTex){
    position.x = 0;
    position.y = 0;
    position.h = h;
    position.w = w;
    
    animationFrame.x = 0;
    animationFrame.y = 0;
    animationFrame.h = h;
    animationFrame.w = w;
    
    xVel = 0;
    yVel = 0;
    
    spriteSheet = spriteTex;
}

//Movement

void Entity::goLeft(){
    xVel = -5;
}

void Entity::goRight(){
    xVel = 5;
}

void Entity::move(){
    position.x += xVel;
    position.y += yVel;
}

void Entity::stopX(){
    xVel = 0;
}

void Entity::stopY(){
    yVel = 0;
}

void Entity::jump(){
    yVel = -15;
}

void Entity::fall(){
	if (yVel < 15)
		yVel += 1;
	
}

//Animation

void Entity::animate(int speed, SDL_Renderer* renderer, int camX, int camY){
    
	
    animationProgress++;
	
    if (animationProgress % speed == 0){
        advance();
    }
	//player is rendered in renderSpace, relative to camera by subtracting offset
	SDL_Rect renderSpace = { position.x - camX, position.y - camY, animationFrame.w, animationFrame.h };
    SDL_RenderCopy(renderer, spriteSheet, &animationFrame, &renderSpace);

}

void Entity::switchAnimationChannel(int channel){
    
    int spriteSheetHeight;
    SDL_QueryTexture(spriteSheet, NULL, NULL, NULL, &spriteSheetHeight);
    
    /* Channels are represented as new rows on a sprite sheet. We move
     * the animationFrame down by 1 unit to switch to a new
     * animation.
     */
    
    if (channel * animationFrame.h < spriteSheetHeight){
        animationFrame.y = channel * animationFrame.h;
    }
}

void Entity::render(SDL_Renderer* renderer, int camX, int camY){

	SDL_Rect renderSpace = { position.x - camX, position.y - camY, animationFrame.w, animationFrame.h };
    SDL_RenderCopy(renderer, spriteSheet, &animationFrame, &renderSpace);
}

void Entity::advance(){
    
    animationFrame.x += animationFrame.w;
    
    int spriteSheetWidth;
    SDL_QueryTexture(spriteSheet, NULL, NULL, &spriteSheetWidth, NULL);
    
    //Loop back to frame one when we reach the edge of the spritesheet
    if (animationFrame.x >= spriteSheetWidth)
        animationFrame.x = 0;
}

//setters
void Entity::setSpriteSheet(SDL_Texture* spriteTex){
    spriteSheet = spriteTex;
}

void Entity::setPosRect(int h, int w, int x, int y){
    position.h = h;
    position.w = w;
    position.x = x;
    position.y = y;
    
    animationFrame.h = h;
    animationFrame.w = w;
}

void Entity::setXPos(int x){
    position.x = x;
}

void Entity::setYPos(int y){
    position.y = y;
}

//getters

SDL_Texture* Entity::getTexture(){
    return spriteSheet;
}

SDL_Rect* Entity::getPos(){
    return &position;
}

double Entity::getxPos(){
    return position.x;
}

double Entity::getyPos(){
    return position.y;
}

double Entity::getYVel() {
	return yVel;
}

int Entity::getHeight(){
    return position.h;
}

int Entity::getWidth(){
    return position.w;
}

int Entity::bottom(){
    return position.y + position.h;
}

int Entity::right(){
    return position.x + position.w;
}



/*
 **********
 PLAYER ***
 **********
 */

Player::Player() : Entity(){
    
}

Player::Player(double x, double y) : Entity(x,y){
    
}


