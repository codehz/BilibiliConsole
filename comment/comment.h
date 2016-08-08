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
    const uint32_t  maxline;
    const uint32_t  height;
    const uint32_t  padding;
    const float     durations;
    const uint32_t  viewport_width;
    const uint32_t  viewport_height;
    CommentManagerConfigure(
        const uint32_t  maxline,
        const uint32_t  height,
        const uint32_t  padding,
        const float     durations,
        const uint32_t  viewport_width,
        const uint32_t  viewport_height
    );
    virtual float scale(float width) const = 0;
    virtual BasicTexture requestTexture(const CommentBase &comment) const = 0;
    virtual void freeTexture(const BasicTexture) const = 0;
    virtual void draw(const BasicTexture, const float x, const float y) const = 0;
};

CommentManager *makeCommentManager(const CommentManagerConfigure *configure);

}
