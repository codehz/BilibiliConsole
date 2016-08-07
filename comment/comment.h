#pragma once
#include <string>
#include <functional>

namespace Bilibili {

enum class CommentType {
    normal = 1,
    bottom = 4,
    top = 5,
    reverse = 6,
    advance = 7,
    script = 8
};

struct CommentBase {
    double time;
    CommentType type;
    uint32_t pt_size;
    uint32_t color;
    uint64_t timestamp;
    uint32_t pool_id;
    uint32_t uid_hash;
    uint32_t comment_id;
    std::string content;
};

struct BasicTexture {
    void *texture;
    uint32_t width;
    uint32_t height;
};

struct CommentManager {
    virtual void pushComment(const CommentBase &comment) = 0;
    virtual void update(double time) = 0;
};

struct CommentManagerConfigure {
    uint32_t  maxline;
    uint32_t  height;
    uint32_t  padding;
    float     durations;
    uint32_t  viewport_width;
    uint32_t  viewport_height;
    std::function<const BasicTexture(const CommentBase &comment)>                 requestTexture;
    std::function<void(const BasicTexture)>                                       freeTexture;
    std::function<void(const BasicTexture, const float x, const float y)>         draw;
};

CommentManager *makeCommentManager(const CommentManagerConfigure &configure);

}
