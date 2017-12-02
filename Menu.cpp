

#include "Menu.h"

Menu::Menu(){
    menuArea = { 0, 0, 0, 0 };
    
    menuTex = nullptr;
    menuFont = nullptr;
    
    fontColor.r = 0;
    fontColor.g = 0;
    fontColor.b = 0;
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
    
    menuOptions.push_back(renderString(renderer, string));
    
}

void Menu::setFont(const char* path, int fontSize){
    
    menuFont = TTF_OpenFont(path, fontSize);
    
    if( menuFont == nullptr )
        std::cout << "Failed to load font: " << TTF_GetError() << "\n";
    
}

void Menu::draw(SDL_Renderer* renderer){
    SDL_RenderCopy(renderer, menuTex, NULL, &menuArea);
    for (int i = 0; i < menuOptions.size(); i++){
        
    }
}

/********************
 * HELPER FUNCTIONS *
 ********************/

SDL_Texture* Menu::renderString(SDL_Renderer* renderer, const char* string){
    //The texture to return
    SDL_Texture* stringTexture;
    
    
    stringTexture = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(menuFont, string, fontColor));
    
    if (stringTexture == nullptr){
        std::cout << "Failed to render text.\n";
        return nullptr;
    }
    
    return stringTexture;
}


