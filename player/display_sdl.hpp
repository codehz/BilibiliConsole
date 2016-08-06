#pragma once
#include "decl_pointer.hpp"
#include "display.hpp"
#include "sdl_text.hpp"
#include <SDL.h>
#include <SDL_mutex.h>
#include <SDL_ttf.h>
#include <exception>
#include <string>

struct sdl_error : public std::runtime_error {
    sdl_error(const char* m) : std::runtime_error(m) {}
};

//SDL_Window
template<>
template<>
SDL_Window *                custom_pointer<SDL_Window>::            Create<BasicInfo>(BasicInfo basicInfo) {
    auto ret = SDL_CreateWindow("BiliBiliConsole Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, basicInfo.vinfo.xres, basicInfo.vinfo.yres, SDL_WINDOW_SHOWN);
    if (!ret) {
        std::cerr << "Couldn't create window:" << SDL_GetError();
        throw sdl_error("Couldn't create window");
    }
    return ret;
}
template<>
void                        custom_pointer<SDL_Window>::            Destroy(SDL_Window *t) {
    SDL_DestroyWindow(t);
}

//SDL_Renderer
template<>
template<>
SDL_Renderer *              custom_pointer<SDL_Renderer>::          Create<custom_pointer<SDL_Window>>(custom_pointer<SDL_Window> window) {
    auto ret = SDL_CreateRenderer(window, -1, 0);
    if (!ret) {
        std::cerr << "Couldn't create renderer:" << SDL_GetError();
        throw sdl_error("Couldn't create renderer");
    }
    return ret;
}
template<>
void                        custom_pointer<SDL_Renderer>::          Destroy(SDL_Renderer *t) {
    SDL_DestroyRenderer(t);
}

//SDL_Texture
template<>
template<>
SDL_Texture *               custom_pointer<SDL_Texture>::           Create<BasicInfo, custom_pointer<SDL_Renderer>>(BasicInfo basicInfo, custom_pointer<SDL_Renderer> renderer) {
    auto ret = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR565, SDL_TEXTUREACCESS_STREAMING, basicInfo.vinfo.xres, basicInfo.vinfo.yres);
    if (!ret) {
        std::cerr << "Couldn't create texture:" << SDL_GetError();
        throw sdl_error("Couldn't create texture");
    }
    return ret;
}
template<>
void                        custom_pointer<SDL_Texture>::           Destroy(SDL_Texture *t) {
    SDL_DestroyTexture(t);
}

//SDL_Surface
template<>
void                        custom_pointer<SDL_Surface>::           Destroy(SDL_Surface *t) {
    SDL_FreeSurface(t);
}

//SDL_mutex
template<>
SDL_mutex *                 custom_pointer<SDL_mutex>::             Create() {
    return SDL_CreateMutex();
}
template<>
void                        custom_pointer<SDL_mutex>::             Destroy(SDL_mutex *t) {
    SDL_DestroyMutex(t);
}

namespace BilibiliConsole {

class SDLInit {
    protected:
        SDLInit() {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                std::cerr << "Could not initialize SDL:" << SDL_GetError();
                throw sdl_error("Could not initialize SDL");
            }
        }

        ~SDLInit() {
            SDL_Quit();
        }
};

typedef SDL_Window      Window;
typedef SDL_Renderer    Renderer;
typedef SDL_Texture     Texture;
typedef SDL_Surface     Surface;
typedef SDL_mutex       Lock;

class SDLDisplay : protected SDLInit, public TextEngine {
    protected:
        BasicInfo                       basicInfo;
        custom_pointer<Window>          window;
        custom_pointer<Renderer>        renderer;
        custom_pointer<Texture>         texture;
        custom_pointer<Lock>            lock;
    public:
        SDLDisplay(std::string font_name) : SDLInit(), basicInfo(), window(basicInfo), renderer(window), texture(basicInfo, renderer), lock(), TextEngine(font_name) {}

        void doLock(void **p_pixels) {
            int pitch;
            SDL_LockMutex(lock);
            SDL_LockTexture(texture, NULL, p_pixels, &pitch);
        }

        void doUnlock(void *const *p_pixels) {
            SDL_UnlockTexture(texture);
            SDL_UnlockMutex(lock);
        }

        void doDisplay() {
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        }
        
        void doSubmit() {
            SDL_RenderPresent(renderer);
        }

        void doLoop() {
            bool done = false;
            SDL_Event event;
            while (!done) {
                int action = 0;
                while (SDL_PollEvent(&event)) {
                    switch(event.type) {
                        case SDL_QUIT:
                            done = true;
                            break;
                        case SDL_KEYUP:
                            action = event.key.keysym.sym;
                            break;
                    }
                }

                switch(action) {
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        done = 1;
                        break;
                }
                SDL_Delay(100);
            }
        }

        WH measureText(std::string text, int pt_size) {
            return TextEngine::measureText(text, pt_size);
        }

        void *renderText(std::string text, int pt_size, Color color) {
            return SDL_CreateTextureFromSurface(renderer, make_custom(TextEngine::renderTextSurface(text, pt_size, color)));
        }

        void drawTo(void *texture, Rect rect) {
            SDL_RenderCopy(renderer, reinterpret_cast<SDL_Texture *>(texture), nullptr, reinterpret_cast<SDL_Rect *>(&rect));
        }
};

using TargetDisplay=SDLDisplay;

}
