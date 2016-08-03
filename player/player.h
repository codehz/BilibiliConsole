#pragma once
#include <string>
#include <memory>
#include "player_info.h"
#include "player_hook.h"

namespace BilibiliConsole {

class Player {
    protected:
        Player() {};
    public:
        virtual void load(std::string location) = 0;
        virtual void play() = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;
        virtual void loop() = 0;
        //virtual const PlayerInfo &state() = 0;
};

}

BilibiliConsole::Player *createPlayer(const BilibiliConsole::PlayerHook &hook);