#pragma once
#include <string>
#include <memory>
#include "player_info.h"
#include "player_hook.h"

namespace BilibiliConsole {

struct WH {
    int w, h;
};

struct Rect {
    int x, y, w, h;
};

struct ITexture {};

struct Color {
    uint8_t r, g, b, a;
};

class Player {
    protected:
        Player() {};
    public:
        virtual void registerHook(BilibiliConsole::PlayerHook *hook) = 0;
        virtual void load(std::string location) = 0;
        virtual void play() = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;
        virtual void loop() = 0;
        virtual WH measureText(std::string text, int pt_size) = 0;
        virtual void *renderText(std::string text, int pt_size, Color color) = 0;
        virtual void drawTo(void *texture, Rect rect) = 0;
        virtual void destoryTexture(void *texture) = 0;
        //virtual const PlayerInfo &state() = 0;
};

}

BilibiliConsole::Player *createPlayer(std::string font_name);
