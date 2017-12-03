

#ifndef Menu_h
#define Menu_h
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <iostream>

class Menu{
    /* Class for management and rendering of in-game menus.
     */
    
public:
    //Values representing menu options
    enum pauseCommand {PAUSE_RESUME, PAUSE_ITEM, PAUSE_QUIT};
    enum battleCommand {BATTLE_FIGHT, BATTLE_ITEM, BATTLE_RUN};
    
    struct menuOption{
        menuOption(SDL_Texture* t, unsigned int l){ text = t; length = l; }
        SDL_Texture* text;
        unsigned int length;
    };
    
    //Default constructor
    Menu();
    
    //Initialize menu with specified dimensions, position, and texture.
    Menu(SDL_Texture* texture, int h, int w, int x = 0, int y = 0);
    
    ~Menu();
    
    //Draw the menu to the screen.
    void draw(SDL_Renderer* renderer);
    void drawBorder(SDL_Renderer* renderer);
    
    //Set the menu's font
    void setFont(const char* path, int fontSize);
    
    //Add an option to the menu
    void addOption(SDL_Renderer* renderer, const char* string);
    
    //move the menu's cursor
    void cursorIncrement();
    void cursorDecrement();
    
    //Getters
    int getCursorPos();
    
private:
    //Representation of the menu's area on the screen.
    SDL_Rect menuArea;
    
    //Body texture for the menu box
    SDL_Texture* menuTex;
    
    //Font in which menu options will be displayed.
    TTF_Font* menuFont;
    
    //Font color in which menu options will be displayed.
    SDL_Color fontColor;
    
    /* Vector of menu options. These are textures created from strings using
     * the ttf utility.
     */
    std::vector<menuOption> menuOptions;
    
    //Index for the menu's cursor
    int cursorPos;
    
    /********************
     * HELPER FUNCTIONS *
     ********************/
    
    /* Converts string to a texture using font menuFont and the specified color.
     * Default color is black.
     */
    SDL_Texture* renderString(SDL_Renderer* renderer, const char* string);
    
};

#endif /* Menu_h */
