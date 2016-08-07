#include "../player/player.h"
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <limits.h>
#include <libgen.h>

using namespace BilibiliConsole;

struct SimplePlayerHook : public PlayerHook {
    Player *player;
    WH textwh;
    void *test_text;
    void onInit(Player *player) {
        this->player = player;
        textwh = player->measureText("FPS", 16);
        test_text = player->renderText("FPS", 16, {255, 255, 0, 255});
    }
    void onDestroy() const {}
    void onDisplay(float fps, float time) const {
        player->drawTo(test_text, {0, 0, textwh.w, textwh.h});
    }
    void onException(std::string exception) const {}
};

std::string getDir() {
    char dest[PATH_MAX];
    if (readlink("/proc/self/exe", dest, PATH_MAX) != -1)
        return std::string(dirname(dest));
    throw "Cannot getcwd";
}

int main(int argc, char *argv[]) {
    SimplePlayerHook hook;
    std::string dir = getDir();
    std::shared_ptr<Player> player(createPlayer(dir + "/res/test.ttf"));
    player->registerHook(&hook);
    player->load("file://" + dir + "/res/test.mp4");
    player->play();
    player->loop();
}
