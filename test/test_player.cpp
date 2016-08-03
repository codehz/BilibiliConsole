#include "../player/player.h"
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstdio>

using namespace BilibiliConsole;

class DummyPlayerHook : public PlayerHook {
    void onFrame(void *const *p_pixels) {
    }
    void onException(std::string exception) {
    }
};

std::string getCurrentWorkingDirectory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        return std::string(cwd);
    throw "Cannot getcwd";
}

int main() {
    DummyPlayerHook hook;
    std::shared_ptr<Player> player(createPlayer(hook));
    printf("VLC_PLUGIN_PATH=%s\n", getenv("VLC_PLUGIN_PATH"));
    player->load("file://" + getCurrentWorkingDirectory() + "/test.mp4");
    player->play();
    player->loop();
}
