#pragma once
#include <string>

namespace BilibiliConsole {

class PlayerHook {
    virtual void onFrame(void *const *p_pixels) = 0;
    virtual void onException(std::string exception) = 0;
};

}