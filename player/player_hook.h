#pragma once
#include <string>

namespace BilibiliConsole {

struct Player;

struct PlayerHook {
    virtual void onInit(Player *) = 0;
    virtual void onDestroy() const = 0;
    virtual void onDisplay(void *) const = 0;
    virtual void onException(std::string exception) const = 0;
};

}
