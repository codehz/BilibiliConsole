#include "player.h"
#include "../utils/custom_pointer.hpp"
#include "../utils/make_callback.hpp"
#include "decl_pointer.hpp"
#include "display.hpp"
#include <iostream>

#include "display_sdl.hpp"

#define MAKE_CALLBACK(name) force_cast<vlc_##name>(&PlayerImpl::name)

namespace BilibiliConsole {

using vlc_lock = void *(*)(void *, void **);
using vlc_unlock = void (*)(void *, void *, void *const *);
using vlc_display = void (*)(void *, void *id);

template <typename SepcialDisplay>
class PlayerImpl : public Player, protected SepcialDisplay {
    private:
        const PlayerHook *                      hook;
        BasicInfo                               basicInfo;
        custom_pointer<libvlc_instance_t>       libvlc;
        custom_pointer<libvlc_media_player_t>   vlcPlayer;
        libvlc_state_t                          playerState;

        void *lock(void **p_pixels) {
            SepcialDisplay::doLock(p_pixels);
        }

        void unlock(void *id, void *const *p_pixels) {
            SepcialDisplay::doUnlock(p_pixels);
        }

        void display(void *id) {
            SepcialDisplay::doDisplay();
            if (hook) hook->onDisplay(libvlc_media_player_get_fps(vlcPlayer), libvlc_media_player_get_time(vlcPlayer) / 1000.0);
            SepcialDisplay::doSubmit();
        }

        void updateState() {
            playerState = libvlc_media_player_get_state(vlcPlayer);
        }
    public:
        PlayerImpl(std::string font_name) : hook(nullptr), SepcialDisplay(font_name), libvlc(), vlcPlayer(nullptr) {
        }
        
        void registerHook(BilibiliConsole::PlayerHook *hook) {
            this->hook = hook;
            if (hook) hook->onInit(this);
        }

        void load(std::string location) {
            custom_pointer<libvlc_media_t> media(libvlc, location);
            vlcPlayer = custom_pointer<libvlc_media_player_t>(media);
            libvlc_video_set_callbacks(vlcPlayer, make_callback(&PlayerImpl::lock), make_callback(&PlayerImpl::unlock), make_callback(&PlayerImpl::display), this);
            libvlc_video_set_format(vlcPlayer, "RV16", basicInfo.vinfo.xres, basicInfo.vinfo.yres, basicInfo.vinfo.xres * 2);
        }

        void play() {
            libvlc_media_player_play(vlcPlayer);
        }

        void pause() {
            if (playerState == libvlc_Playing)
                libvlc_media_player_set_pause(vlcPlayer, true);
            else if (playerState == libvlc_Paused)
                libvlc_media_player_set_pause(vlcPlayer, false);
        }

        void stop() {
            libvlc_media_player_stop(vlcPlayer);
        }

        void loop() {
            SepcialDisplay::doLoop();
        }

        WH measureText(std::string text, int pt_size) {
            return SepcialDisplay::measureText(text, pt_size);
        }

        void *renderText(std::string text, int pt_size, Color color) {
            return SepcialDisplay::renderText(text, pt_size, color);
        }
        
        void drawTo(void *texture, Rect rect) {
            return SepcialDisplay::drawTo(texture, rect);
        }

        void destoryTexture(void *texture) {
            return SepcialDisplay::destoryTexture(texture);
        }
};

}

BilibiliConsole::Player *createPlayer(std::string font_name) {
    return new BilibiliConsole::PlayerImpl<BilibiliConsole::TargetDisplay>(font_name);
}

#undef MAKE_CALLBACK
