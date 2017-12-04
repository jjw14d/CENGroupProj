
#include "Entity.h"

/*
 **********
 ENTITY ***
 **********
 */

Entity::Entity(){
    position = {0,0,0,0};
    savedPosition = {0,0,0,0};
    
    
    animationFrame.x = 0;
    animationFrame.y = 0;
    animationFrame.w = 0;
    animationFrame.h = 0;
    spriteSheet = nullptr;
    
    xVel = 0;
    yVel = 0;
    
    hitPoints = 10;
}


Entity::Entity(double h, double w, SDL_Texture* spriteTex){
    position.x = 0;
    position.y = 0;
    position.h = h;
    position.w = w;
    
    savedPosition = {0,0,0,0};
    
    animationFrame.x = 0;
    animationFrame.y = 0;
    animationFrame.h = h;
    animationFrame.w = w;
    
    xVel = 0;
    yVel = 0;
    
    hitPoints = 10;
    
    spriteSheet = spriteTex;
}

//Movement

void Entity::goLeft(){
    xVel = -5;
}

void Entity::goRight(){
    xVel = 5;
}

void Entity::updatePos(){
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

int Entity::animate(int speed, SDL_Renderer* renderer, int camX, int camY){
    
    int result = 0;
    animationProgress++;
	
    if (animationProgress % speed == 0){
        result = advance();
    }
	//player is rendered in renderSpace, relative to camera by subtracting offset
	SDL_Rect renderSpace = { position.x - camX, position.y - camY, animationFrame.w, animationFrame.h };
    SDL_RenderCopy(renderer, spriteSheet, &animationFrame, &renderSpace);
    
    return result;

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

int Entity::advance(){
    
    animationFrame.x += animationFrame.w;
    
    int spriteSheetWidth;
    SDL_QueryTexture(spriteSheet, NULL, NULL, &spriteSheetWidth, NULL);
    
    //Loop back to frame one when we reach the edge of the spritesheet
    if (animationFrame.x >= spriteSheetWidth){
        animationFrame.x = 0;
        return 1;
    }
    return 0;
}

void Entity::savePosition(){
    savedPosition = position;
}

void Entity::loadPosition(){
    position = savedPosition;
}

/***********
 * SETTERS *
 ***********/
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

void Entity::decrementHitPoints(int decrement){
    hitPoints -= decrement;
    if (hitPoints < 0)
        hitPoints = 0;
}

void Entity::incrementHitPoints(int increment){
    hitPoints += increment;
    if (hitPoints > 10)
        hitPoints = 0;
}

/***********
 * GETTERS *
 ***********/

SDL_Texture* Entity::getTexture(){
    return spriteSheet;
}

SDL_Rect* Entity::getPos(){
    return &position;
}

int Entity::getxPos(){
    return position.x;
}

int Entity::getyPos(){
    return position.y;
}

double Entity::getXVel() {
	return xVel;
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

int Entity::getHP(){
    return hitPoints;
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

/*
 ***********
 MONSTER ***
 ***********
 */

Monster::Monster() : Entity(){
    
}

Monster::Monster(double x, double y) : Entity(x,y){
    
}

Monster::~Monster(){
    
}

/*
 ***********
 FLOATER ***
 ***********
 */

Floater::Floater() : Monster(){
    anchor.first = 0;
    anchor.second = 0;
    xVel = 1;
}

Floater::Floater(double x, double y) : Monster(x,y){
    anchor.first = x;
    anchor.second = y;
    xVel = 10;
}

Floater::~Floater(){
    
}

void Floater::move(){
    //check distance from the anchor
    xVel -= (position.x - anchor.first)/100;
}

void Floater::setXPos(int x){
    Entity::setXPos(x);
    anchor.first = x;
}

void Floater::setYPos(int y){
    Entity::setYPos(y);
    anchor.second = y;
    
}


