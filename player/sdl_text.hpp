#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <unordered_map>
#include <string>
#include "player.h"
#include "../utils/custom_pointer.hpp"
#include "../utils/force_cast.hpp"
#include "display_sdl.hpp"

//TTF_Font
template<>
template<>
TTF_Font *                  custom_pointer<TTF_Font>::              Create<std::string, int>(std::string font_name, int pt_size) {
    return TTF_OpenFont(font_name.c_str(), pt_size);
}
template<>
void                        custom_pointer<TTF_Font>::              Destroy(TTF_Font *t) {
    TTF_CloseFont(t);
}

namespace BilibiliConsole {

struct TTFInit {
    TTFInit() {
        TTF_Init();
    }
    
    ~TTFInit() {
        TTF_Quit();
    }
};

class SmartFont {
    private:
        std::unordered_map<int, custom_pointer<TTF_Font>> cache;
        const std::string font_name;
    public:
        SmartFont(std::string font_name) : font_name(font_name) {}
        
        TTF_Font *get(const int pt_size) {
            auto got = cache.find(pt_size);
            if (got == cache.end()) {
                cache.emplace(std::piecewise_construct, std::make_tuple(pt_size), std::make_tuple(font_name, pt_size));
                return get(pt_size);
            }
            return got->second;
        }
};

class TextEngine : private TTFInit, protected SmartFont {
    public:
        TextEngine(std::string font_name) : SmartFont(font_name) {}

        WH measureText(std::string text, int pt_size) {
            WH wh;
            TTF_SizeUTF8(SmartFont::get(pt_size), text.c_str(), &wh.w, &wh.h);
            return wh;
        }

        SDL_Surface *renderTextSurface(std::string text, int pt_size, Color color) {
            return TTF_RenderUTF8_Blended(SmartFont::get(pt_size), text.c_str(), force_cast<SDL_Color>(color));
        }
};

}

