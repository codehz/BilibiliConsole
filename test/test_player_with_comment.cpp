#include "../player/player.h"
#include "../comment/comment.h"
#include "../comment/comment_parser.h"
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <limits.h>
#include <libgen.h>
#include <cmath>
#include <time.h>
#include <stdlib.h>

using namespace BilibiliConsole;

struct MyPlayerHook : public PlayerHook {
    Player *player;
    WH textwh;
    Bilibili::CommentManager *cm;
    MyPlayerHook(Bilibili::CommentManager *cm) : cm(cm) {
    }
    void *test_text;
    void onInit(Player *player) {
        this->player = player;
        textwh = player->measureText("FPS", 16);
        test_text = player->renderText("FPS", 16, {255, 255, 0, 255});
    }
    void onDestroy() const {}
    void onDisplay(float fps, float time) const {
        player->drawTo(test_text, {0, 0, textwh.w, textwh.h});
        //std::cout << time << std::endl;
        cm->update(time);
    }
    void onException(std::string exception) const {}
};

struct MyConfigure : public Bilibili::CommentManagerConfigure {
    std::shared_ptr<Player> player;
    MyConfigure(std::shared_ptr<Player> player) : player(player) {
        maxline = 20;
        height = 30;
        padding = 2;
        durations = 6.0f;
        viewport_width = 800;
        viewport_height = 600;
        requestTexture = [=](auto comment) {return this->requestTextureF(comment);};
        freeTexture = [=](auto texture) {this->freeTextureF(texture);};
        draw = [=](auto texture, auto x, auto y){this->drawF(texture, x, y);};
    }
    
    Bilibili::BasicTexture requestTextureF(const Bilibili::CommentBase &comment) {
        WH textwh = player->measureText(comment.content, 25);
        return {player->renderText(comment.content, 25, {rand() % 255, rand() % 255, rand() % 255, 255}), (uint32_t)textwh.w, (uint32_t)textwh.h};
    }
    
    void freeTextureF(const Bilibili::BasicTexture &texture) {
        player->destoryTexture(texture.texture);
    }
    
    void drawF(const Bilibili::BasicTexture texture, const float x, const float y) {
        player->drawTo(texture.texture, {(int32_t)x, (int32_t)y, (int32_t)texture.width, (int32_t)texture.height});
        //std::cout << "x: " << std::setw(8)  << (int32_t)x << ", y: " << std::setw(8) << (int32_t)y << std::endl;
    }
};

std::string getDir() {
    char dest[PATH_MAX];
    if (readlink("/proc/self/exe", dest, PATH_MAX) != -1)
        return std::string(dirname(dest));
    throw "Cannot getcwd";
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    std::string             dir = getDir();
    std::shared_ptr<Player> player(createPlayer(dir + "/res/test.ttf"));
    MyConfigure             configure(player);
    Bilibili::CommentManager *cm = Bilibili::makeCommentManager(configure);
    MyPlayerHook hook(cm);
    player->registerHook(&hook);
    auto cbs = Bilibili::comment_parse(dir + "/res/test.xml");
    while (!cbs.empty()) {
        cm->pushComment(cbs.front());
        cbs.pop();
    }
    player->load("file://" + dir + "/res/test.mp4");
    player->play();
    player->loop();
}
