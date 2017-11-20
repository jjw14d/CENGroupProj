
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


/*
 **********
 PLAYER ***
 **********
 */

Player::Player() : Entity()
{
	level = 1;
	currentExp = 0; //start with a new character at level 1 with 0 experience
	levelUpExp = 100;
	currentHealth = 100;
	maxHealth = 100;
	baseAttack = 10;
	baseDefence = 10;   
}

Player::Player(double x, double y) : Entity(x,y)
{
	level = 1;
	currentExp = 0;
	levelUpExp = 100;
	currentHealth = 100;
	maxHealth = 100;
	baseAttack = 10;
	baseDefence = 10;   
}

int Player::attack()
{
	return baseAttack;	
}

void Player::getHit(int damage)
{
	currentHealth = currentHealth - damage;
}

bool Player::checkLevelUp()
{
	if (currentExp >= levelUpExp)
		return true;
	else if (currentExp < levelUpExp)
		return false;
}

void Player::levelUp()
{
	level++; //increment the player level
	maxHealth = maxHealth + 10; 
	currentHealth = currentHealth + 10;//health boost for level up, but not a full heal
	baseAttack = baseAttack + 2;
	baseDefence = baseDefence + 2;
	levelUpExp = levelUpExp + 25; //raise the bar to level up
}

void Player::addExp(int expValue)
{
	currentExp = currentExp + expValue;
	if (checkLevelUp())
	{
		currentExp = currentExp % (levelUpExp + 25); //make sure they don't lose experience past a level
		levelUp();
	}
}

int Player::getLevel()
{
	return level;
}

int Player::getCurrentExp()
{
	return currentExp;
}

int Player::getlevelUpExp()
{
	return levelUpExp;
}

int Player::getHealth()
{
	return currentHealth;
}

int Player::getBaseAttack()
{
	return baseAttack;
}

int Player::getBaseDefence()
{
	return baseDefence;
}

int Player::getMaxHealth()
{
	return maxHealth;
}

/*Monster*/

Monster::Monster() : Entity()
{
	//generate default level 1 monsters
	currentHealth = 25;
	maxHealth = 25;
	baseAttack = 5;
	baseDefence = 5; //default values for combat stats
	expValue = 25;
}

Monster::Monster(double x, double y, SDL_Texture* spriteTex, int playerLevel) : Entity(x, y, spriteTex)
{
	maxHealth = 20 + (playerLevel * 5); //scale stats with player level
	currentHealth = maxHealth;
	baseAttack = 3 + (playerLevel * 2);
	baseDefence = 3 + (playerLevel * 2);
	expValue = 20 + (playerLevel * 5);
}

int Player::attack()
{
	return baseAttack;
}

void Monster::getHit(int damage)
{
	currentHealth = currentHealth - damage;
}

int Monster::getHealth()
{
	return currentHealth;
}

int Monster::getBaseAttack()
{
	return baseAttack;
}

int Monster::getBaseDefence()
{
	return baseDefence;
}

int Monster::getmaxHealth()
{
	return maxHealth;
}

int Monster::getExpVal()
{
	return expValue;
}
