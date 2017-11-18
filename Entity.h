
#ifndef Entity_h
#define Entity_h

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

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
    void move();
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
     * provided renderer.
     */
    
    void animate(int speed, SDL_Renderer* renderer, int camX, int camY);
    
    //Re-render the player without animation
    
    void render(SDL_Renderer* renderer, int camX, int camY);
    
    //Adjusts the source rectangle used in rendering to a different animation track
    
    void switchAnimationChannel(int channel);
    
    //Advance the frame of the current animation channel by one
    
    void advance();
    
    
    
    /***********
     * SETTERS *
     ***********/
    void setSpriteSheet(SDL_Texture* spriteTex);
    void setPosRect(int h, int w, int x = 0, int y = 0);
    void setXPos(int x);
    void setYPos(int y);
    
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
     * frame in the advance() fucntion.
     */
    
    SDL_Rect animationFrame;
    
    //SDL_Rect to render the object's sprite to.
    
    SDL_Rect position;
    
    //Velocity
    
    int xVel;
    int yVel;
};

class Player : public Entity{
public:
    Player();
    
    //Constructor to initialize player position
    
    Player(double x, double y);
 
private:
    
};


#endif /* Entity_h */
