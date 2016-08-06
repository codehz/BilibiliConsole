#include "../comment/comment.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

Bilibili::CommentManagerConfigure configure = {
    .maxline = 30,
    .height = 30,
    .padding = 2,
    .durations = 6.0f,
    .viewport_width = 800,
    .viewport_height = 600,
    .requestTexture = [](const Bilibili::CommentBase &comment) -> Bilibili::BasicTexture {
        return {nullptr, comment.content.length() * 16, 16};
    },
    .freeTexture = [](const Bilibili::BasicTexture &texture) {},
    .draw = [](const Bilibili::BasicTexture, const float x, const float y) {
        std::cout << "x: " << std::setw(8)  << (int32_t)x << ", y: " << std::setw(8) << (int32_t)y << std::endl;
    }
};

int main() {
    Bilibili::CommentManager *cm = Bilibili::makeCommentManager(configure);
    Bilibili::CommentBase cbs[5] = {{
        .time = 0.4f,
        .type = Bilibili::CommentType::normal,
        .pt_size = 16,
        .color = 0xFFFFFFFF,
        .timestamp = 0,
        .pool_id = 0,
        .uid_hash = 0,
        .comment_id = 0,
        .content = "1"
    }, {
        .time = 4.5f,
        .type = Bilibili::CommentType::normal,
        .pt_size = 16,
        .color = 0xFFFFFFFF,
        .timestamp = 0,
        .pool_id = 0,
        .uid_hash = 0,
        .comment_id = 0,
        .content = "1"
    }, {
        .time = 3.2f,
        .type = Bilibili::CommentType::top,
        .pt_size = 16,
        .color = 0xFFFFFFFF,
        .timestamp = 0,
        .pool_id = 0,
        .uid_hash = 0,
        .comment_id = 0,
        .content = "1"
    }, {
        .time = 7.4f,
        .type = Bilibili::CommentType::normal,
        .pt_size = 16,
        .color = 0xFFFFFFFF,
        .timestamp = 0,
        .pool_id = 0,
        .uid_hash = 0,
        .comment_id = 0,
        .content = "1"
    }, {
        .time = 2.3f,
        .type = Bilibili::CommentType::normal,
        .pt_size = 16,
        .color = 0xFFFFFFFF,
        .timestamp = 0,
        .pool_id = 0,
        .uid_hash = 0,
        .comment_id = 0,
        .content = "1"
    }};
    for (auto cb : cbs)
        cm->pushComment(cb);
    for (auto i = 0.0f; i < 14; i+=0.02) {
        std::cout << "\033[H\033[Jtime: " << i << std::endl;
        cm->update(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    delete cm;
}
