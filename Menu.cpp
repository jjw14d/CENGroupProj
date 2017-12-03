

#include "Menu.h"

Menu::Menu(){
    menuArea = { 0, 0, 0, 0 };
    
    menuTex = nullptr;
    menuFont = nullptr;
    
    fontColor.r = 0;
    fontColor.g = 0;
    fontColor.b = 0;
    
    cursorPos = 0;
    
}

Menu::Menu(SDL_Texture* texture, int h, int w, int x, int y){
    menuArea.h = h;
    menuArea.w = w;
    menuArea.x = x;
    menuArea.y = y;
    
    menuTex = texture;
    menuFont = nullptr;
    
    fontColor.r = 0;
    fontColor.g = 0;
    fontColor.b = 0;
    
    cursorPos = 0;
}

Menu::~Menu(){
    if (menuTex != nullptr)
        SDL_DestroyTexture(menuTex);
    if (menuFont != nullptr)
        TTF_CloseFont(menuFont);
}

/* Renders a new texture from the provided string and
 * adds it to the list of menuOptions. The renderer must
 * be passed in, as it is required for texture generation.
 */
void Menu::addOption(SDL_Renderer* renderer, const char* string){
    
    menuOptions.push_back(menuOption(renderString(renderer, string), strlen(string)));
    
}

void Menu::setFont(const char* path, int fontSize){
    
    menuFont = TTF_OpenFont(path, fontSize);
    
    if( menuFont == nullptr )
        std::cout << "Failed to load font: " << TTF_GetError() << "\n";
    
}

void Menu::draw(SDL_Renderer* renderer){
    
    //Save old draw color
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);;
    
    //Draw background texture
    SDL_RenderCopy(renderer, menuTex, NULL, &menuArea);
    
    //Frame to render the text in. Moves down for each entry.
    SDL_Rect textRect = {menuArea.x+50, menuArea.y+20, 10, 10};
    
    //Render menu options
    for (int i = 0; i < menuOptions.size(); i++){
        textRect.w = menuOptions[i].length * 10;
        SDL_RenderCopy(renderer, menuOptions[i].text, NULL, &textRect);
        textRect.y += textRect.h + 10;
    }
    
    //Draw cursor at the proper menu index
    SDL_Rect cursorRect = {menuArea.x+25, menuArea.y+20+(cursorPos*20),10,10};
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawRect(renderer, &cursorRect);
    
    drawBorder(renderer);
    
    //restore old draw color
    SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);

}

void Menu::drawBorder(SDL_Renderer* renderer){
    //Save old draw color
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
    
    //Border is black
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    
    SDL_Rect topBorder = {menuArea.x - 5,
                             menuArea.y - 5,
                             menuArea.w + 10,
                             10};
    SDL_Rect bottomBorder = {menuArea.x - 5,
                                menuArea.y + menuArea.h - 5,
                                menuArea.w + 10,
                                10};
    SDL_Rect leftBorder = {menuArea.x - 5,
                              menuArea.y - 5,
                              10,
                              menuArea.h + 10};
    SDL_Rect rightBorder = {menuArea.x + menuArea.w - 5,
                                menuArea.y - 5,
                                10,
                                menuArea.h + 10};
    
        
    
    SDL_RenderFillRect(renderer, &topBorder);
    SDL_RenderFillRect(renderer, &bottomBorder);
    SDL_RenderFillRect(renderer, &leftBorder);
    SDL_RenderFillRect(renderer, &rightBorder);
    
    
    //Restore old draw color
    SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

void Menu::cursorIncrement(){
    if (cursorPos < menuOptions.size() - 1)
        cursorPos++;
    else
        cursorPos = 0;
}

void Menu::cursorDecrement(){
    if (cursorPos > 0)
        cursorPos--;
    else
        cursorPos = menuOptions.size() - 1;
}

//Getters
int Menu::getCursorPos(){
    return cursorPos;
}

/********************
 * HELPER FUNCTIONS *
 ********************/

SDL_Texture* Menu::renderString(SDL_Renderer* renderer, const char* string){
    //The texture to return
    SDL_Texture* stringTexture;
    
    stringTexture = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(menuFont, string, fontColor));
    
    if (stringTexture == nullptr){
        std::cout << "Failed to render menu text.\n";
        return nullptr;
    }
    
    return stringTexture;
}


