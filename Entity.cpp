
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
    //if (hitPoints > 10)
        //hitPoints = 0;
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

int Entity::getBaseAttack()
{
    return baseAttack;
}

int Entity::getBaseDefense()
{
    return baseDefense;
}

/*
 **********
 PLAYER ***
 **********
 */

Player::Player() : Entity(){
    //set stats for new game
	level = 1;
	hitPoints = 50;
	maxHitPoints = 50;
	baseAttack = 10;
	baseDefense = 10;
	exp = 0;
	expToLevelUp = 25;
}

Player::Player(double x, double y) : Entity(x,y){
    //set stats for new game
	level = 1;
	hitPoints = 50;
	maxHitPoints = 50;
	baseAttack = 10;
	baseDefense = 10;
	exp = 0;
	expToLevelUp = 25;
}

bool Player::attackSuccess()
{
	int roll = ((rand() % 100) + 1); //get a random number between 1 and 100
	int missThreshold = 95; //base 95% chance to hit
	if (roll < missThreshold)
		return true;
	else
		return false;
}

int Player::attack()
{
	int roll = ((rand() % 20) + 1); //get a random number between 1 and 20
	if (roll < 20)
	{
		roll = roll - 10;
		return (baseAttack + roll); //do bonus damage on rolls above 10, damage penalty on rolls below
	}
	else if (roll == 20)
		return (baseAttack * 2); //Critical Hit!
}

void Player::levelUp()
{
	if (exp >= expToLevelUp)
	{
		exp = exp % expToLevelUp; //don't lose extra experience
		level++;
		maxHitPoints = maxHitPoints + 10;
		hitPoints = hitPoints + 10; //give health boost for leveling up
		baseAttack = baseAttack + 5;
		baseDefense = baseDefense + 5;
		expToLevelUp = expToLevelUp + 25;
	}
}

void Player::getHit(int damage)
{
	if (damage > baseDefense)
	{
        decrementHitPoints(damage - baseDefense);
	}
}

void Player::addExp(int expValue)
{
	exp = exp + expValue;
	levelUp();
}

//accessors

int Player::getLevel()
{
	return level;
}
int Player::getMaxHitPoints()
{
	return maxHitPoints;
}
int Player::getCurrentExp()
{
	return exp;
}

int Player::getExpToLevelUp()
{
	return expToLevelUp;
}

/*
 ***********
 MONSTER ***
 ***********
 */

Monster::Monster() : Entity(){
    	hitPoints = 50;
	baseAttack = 20;
	expValue = 25; //points for defeating it
}

Monster::Monster(double x, double y) : Entity(x,y){
    	hitPoints = 50;
	baseAttack = 20;
	expValue = 25; //points for defeating it
}

Monster::~Monster(){
    
}

bool Monster::attackSuccess(Player &player)
{
	int roll = ((rand() % 100) + 1); //get a random number between 1 and 100
	int missThreshold = 80 - (player.getBaseDefense()); //take player's defense into account, with a base 80% chance to hit
	if (roll < missThreshold)
		return true;
	else
		return false;
}

int Monster::attack()
{
	int roll = ((rand() % 20) + 1); //get a random number between 1 and 20
	roll = roll - 10;
	return (baseAttack + roll); //do bonus damage on rolls above 10, damage penalty on rolls below
}

void Monster::getHit(int damage)
{
    decrementHitPoints(damage);
}

void Monster::healSelf()
{
	int roll = ((rand() % 100) + 1); 
	int healThreshold = 30; //give the monster a 30% chance to heal itself
	if (roll < healThreshold)
	{
		roll = ((rand() % 5) + 1); //between 1 and 5
		incrementHitPoints(5 + roll);
	}
}

int Monster::getExpValue()
{
	return expValue;
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


