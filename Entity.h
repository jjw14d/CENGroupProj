
#ifndef Entity_h
#define Entity_h

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <tuple>
#include <iostream>
/* Defines parameters for any moving objects that can interact with one another.
 * this includes monsters as well as the player character.
 */
class Entity{
    
public:

    Entity();
    
    //Constructor to initalize entity dimensions and spriteSheet
    
    Entity(double x, double y, SDL_Texture* spriteTex = nullptr);
    
    /************
     * MOVEMENT *
     ************/
    
    void goLeft();
    void goRight();
    void updatePos();
    void jump();
    void stopX();
    void stopY();
    void fall();
    
    /*************
     * ANIMATION *
     *************/
    
    /* Animate the sprite. Will advance the sprite only if animationProgress % speed == 0.
     * In other words, the lower the speed, the faster the animation, with 1 being the fastest.
     * If it updates the frame, the function will call SDL_renderCopy with the
     * provided renderer. Returns 1 if the animation loops back to frame 1, 0 otherwise.
     */
    
    int animate(int speed, SDL_Renderer* renderer, int camX, int camY);
    
    //Re-render the player without animation
    
    void render(SDL_Renderer* renderer, int camX, int camY);
    
    //Adjusts the source rectangle used in rendering to a different animation track
    
    void switchAnimationChannel(int channel);
    
    /* Advance the frame of the current animation channel by one.
     *  returns 1 if the loop is reset, and 0 otherwise.
     */
     
    int advance();
    
    //Save the Entity's current position to the "savedPosition" member.
    
    void savePosition();
    
    //Load savedPosition to position/
    
    void loadPosition();
    
    /***********
     * SETTERS *
     ***********/
    void setSpriteSheet(SDL_Texture* spriteTex);
    void setPosRect(int h, int w, int x = 0, int y = 0);
    void setXPos(int x);
    void setYPos(int y);
    void decrementHitPoints(int decrement);
    void incrementHitPoints(int increment);
    

    /***********
     * GETTERS *
     ***********/
    int getxPos();
    int getyPos();
	double getYVel();
	double getXVel();
    int getHeight();
    int getWidth();
    int bottom();
    int right();

    int getHP();
    int getBaseAttack();
    int getBaseDefence();

    SDL_Texture* getTexture();
    SDL_Rect* getPos();

protected:
    //Spritesheet containing animation frames
    
    SDL_Texture* spriteSheet;
    
    /* Progress of the animation. This is used in the animate function to determine
     * when to advance the animation frame
     */
    
    int animationProgress;
    
    /* animationFrame refers to a cropped area of the spritesheet from which to render.
     * usually, it encompasses a single frame of animation, and is moved to advance the
     * frame in the advance() function.
     */
    
    SDL_Rect animationFrame;
    
    //SDL_Rect to render the object's sprite to.
    
    SDL_Rect position;
    
    //Temporary SDL_Rect to save the object's position when we switch to battle mode.
    
    SDL_Rect savedPosition;
    
    //Velocity
    
    int xVel;
    int yVel;
    
    //Battle related info
    int hitPoints;
    int baseAttack;
    int baseDefence;
};

class Player : public Entity{
public:
    Player();
    
    //Constructor to initialize player position
    
    Player(double x, double y);
	
	int attack(); //return a damage value based on player's attack
	void levelUp(); //will run inside addExp, to level up automatically
	void getHit(int damage);
	void addExp(int expValue);

	//accessors
	int getLevel();
	int getMaxHitPoints();
	
	int getCurrentExp();
	int getExpToLevelUp();
 
private:
	
	//battle data
	int level;
	int maxHitPoints;
	
	int exp;
	int expToLevelUp;
    
};

class Monster : public Entity{
public:
    Monster();
    Monster(double x, double y);
    virtual ~Monster();
    
    // Updates the monster's velocity according to its behavior.
    
    virtual void move() = 0;
	int attack(); //return damage based on monster attack
	void getHit(int damage);

	//accessors
	int getExpValue();
    
private:
	int expValue;

};

class Floater : public Monster{
public:
    Floater();
    Floater(double x, double y);
    ~Floater();
    
    //Update velocity based on the difference between the Floater's position and its anchor.
    void move();
    
    
    //Updates anchor point
    void setXPos(int x);
    void setYPos(int y);
    
private:
    
    //An X,Y coordinate pair representing the anchor position for the floater's movement.
    std::pair<int,int> anchor;
    
};


#endif /* Entity_h */
