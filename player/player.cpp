#include "player.h"
#include "../utils/custom_pointer.hpp"
#include "../utils/force_cast.hpp"
#include "decl_pointer.hpp"
#include <iostream>

#define MAKE_CALLBACK(name) force_cast<vlc_##name>(&PlayerImpl::name)

namespace BilibiliConsole {

class SDLInit {
    protected:
        SDLInit() {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                std::cerr << "Could not initialize SDL:" << SDL_GetError();
                throw "Could not initialize SDL";
            }
        }
        
        ~SDLInit() {
            SDL_Quit();
        }
};

using vlc_lock = void *(*)(void *, void **);
using vlc_unlock = void (*)(void *, void *, void *const *);
using vlc_display = void (*)(void *, void *id);

class PlayerImpl : public Player, SDLInit {
    private:
        const PlayerHook &                  hook;
        BasicInfo                           basicInfo;
        custom_pointer<libvlc_instance_t>   libvlc;
        custom_pointer<SDL_Window>          window;
        custom_pointer<SDL_Renderer>        renderer;
        custom_pointer<SDL_Texture>         texture;
        custom_pointer<SDL_mutex>           mutex;
        custom_pointer<libvlc_media_player_t> vlcPlayer;

        nullptr_t lock(void **p_pixels) {
            int pitch;
            SDL_LockMutex(mutex);
            SDL_LockTexture(texture, NULL, p_pixels, &pitch);
            return nullptr;
        }

        void unlock(void *id, void *const *p_pixels) {
            SDL_UnlockTexture(texture);
            SDL_UnlockMutex(mutex);
        }

        void display(void *id) {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }
    public:
        PlayerImpl(const PlayerHook &hook) : hook(hook), window(basicInfo), renderer(window), texture(basicInfo, renderer), vlcPlayer(nullptr) {}

        void load(std::string location) {
            custom_pointer<libvlc_media_t> media(libvlc, location);
            vlcPlayer = custom_pointer<libvlc_media_player_t>(media);
            libvlc_video_set_callbacks(vlcPlayer, MAKE_CALLBACK(lock), MAKE_CALLBACK(unlock), MAKE_CALLBACK(display), this);
            libvlc_video_set_format(vlcPlayer, "RV16", basicInfo.vinfo.xres, basicInfo.vinfo.yres, basicInfo.vinfo.xres * 2);
        }

        void play() {
            libvlc_media_player_play(vlcPlayer);
        }

        void pause() {
        }

        void stop() {
        }

        void loop() {
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
                SDL_Delay(1000/10);
            }
        }
};

}

BilibiliConsole::Player *createPlayer(const BilibiliConsole::PlayerHook &hook) {
    return new BilibiliConsole::PlayerImpl(hook);
}

#undef MAKE_CALLBACK
