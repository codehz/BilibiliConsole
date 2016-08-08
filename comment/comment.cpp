#include "comment.h"
#include <exception>
#include <algorithm>
#include <memory>
#include <map>
#include <queue>
#include <vector>

namespace Bilibili {

CommentManagerConfigure::CommentManagerConfigure(
    const uint32_t  maxline,
    const uint32_t  height,
    const uint32_t  padding,
    const float     durations,
    const uint32_t  viewport_width,
    const uint32_t  viewport_height
) : maxline(maxline), height(height), padding(padding), durations(durations), viewport_width(viewport_width), viewport_height(viewport_height) {}

struct Comment : public CommentBase {
    Comment(const CommentBase &base) : CommentBase(base) {}

    bool operator>(const Comment &rhs) const {
        return time > rhs.time;
    }
};

struct CommentState {
    float   x;
    float   y;
    bool    availability;
};

struct CommentObject {
    const Comment   source;
    const uint32_t  pos;
    const uint32_t  padding;
    const BasicTexture texture;
    CommentState    state;
    bool            need_remove = false;

    CommentObject(const Comment &source, const uint32_t pos, uint32_t padding, const BasicTexture texture)
        : source(source), pos(pos), padding(padding), texture(texture) {}

    void updateState(const double time, const CommentManagerConfigure *configure) {
        throw std::logic_error("Should be subclass");
    }
};

struct ScorllableCommentObject : public CommentObject {
    ScorllableCommentObject(const Comment &source, const uint32_t pos, uint32_t padding, const BasicTexture texture)
        : CommentObject(source, pos, padding, texture) {}
    void updateState(const double time, const CommentManagerConfigure *configure) {
        auto offset_time = time - source.time;
        auto p = offset_time / configure->durations;
        if (p > 1) {
            need_remove = true;
            return;
        }
        state.x = (configure->viewport_width + configure->scale(texture.width)) * p - configure->scale(texture.width);
        state.y = configure->padding + configure->height * pos;
        state.availability = state.x - (configure->scale(texture.width) - texture.width) > 0;
        return;
    }
};

struct ReverseScorllableCommentObject : public CommentObject {
    ReverseScorllableCommentObject(const Comment &source, const uint32_t pos, uint32_t padding, const BasicTexture texture)
        : CommentObject(source, pos, padding, texture) {}
    void updateState(const double time, const CommentManagerConfigure *configure) {
        auto offset_time = time - source.time;
        auto p = offset_time / configure->durations;
        if (p > 1)
            need_remove = true;
        state.x = (configure->viewport_width + configure->scale(texture.width)) * (1 - p) - configure->scale(texture.width);
        state.y = configure->padding + configure->height * pos;
        state.availability = (state.x + configure->scale(texture.width)) < configure->viewport_width;
        return;
    }
};

struct StationaryCommentObject : public CommentObject {
    StationaryCommentObject(const Comment &source, const uint32_t pos, uint32_t padding, const BasicTexture texture)
        : CommentObject(source, pos, padding, texture) {}
    void updateState(const double time, const CommentManagerConfigure *configure) {
        auto offset_time = time - source.time;
        auto p = offset_time / configure->durations;
        if (p > 1)
            need_remove = true;
        state.x = 0.5f * (configure->viewport_width - texture.width);
        state.y = configure->padding + configure->height * pos;
        state.availability = false;
        return;
    }
};

struct ReverseStationaryCommentObject : public CommentObject {
    ReverseStationaryCommentObject(const Comment &source, const uint32_t pos, uint32_t padding, const BasicTexture texture)
        : CommentObject(source, pos, padding, texture) {}
    void updateState(const double time, const CommentManagerConfigure *configure) {
        auto offset_time = time - source.time;
        auto p = offset_time / configure->durations;
        if (p > 1)
            need_remove = true;
        state.x = 0.5f * (configure->viewport_width - texture.width);
        state.y = configure->viewport_height - (configure->padding + configure->height * pos);
        state.availability = false;
        return;
    }
};

class BaseCommentPool {
    protected:
        std::priority_queue<Comment, std::vector<Comment>, std::greater<Comment>> comments;
    public:
        virtual void pushComment(const Comment &comment) = 0;

        virtual void update(const double time, const CommentManagerConfigure *configure) = 0;
};

template<typename CommentObjectType>
class CommentPool : public BaseCommentPool {
    private:
        std::deque<CommentObjectType> current_comments;
    public:
        void pushComment(const Comment &comment) {
            comments.push(comment);
        }

        void update(const double time, const CommentManagerConfigure *configure) {
            std::vector<int> slots(configure->maxline, 0);
            for (auto &comment : current_comments) {
                comment.updateState(time, configure);
                configure->draw(comment.texture, comment.state.x, comment.state.y);
                slots[comment.pos] += 1 - comment.state.availability;
            }
            while (!current_comments.empty() && current_comments.front().need_remove) {
                configure->freeTexture(current_comments.front().texture);
                current_comments.pop_front();
            }
            uint32_t level = *std::min_element(slots.begin(), slots.end());
            auto cursor = slots.begin();
            size_t drawed = current_comments.size();
            while (!comments.empty() && comments.top().time < time) {
                while (*cursor > level) {
                    if (++cursor == slots.end()) {
                        level++;
                        cursor = slots.begin();
                    }
                }
                current_comments.push_back(CommentObjectType(comments.top(), cursor - slots.begin(), configure->padding, configure->requestTexture(comments.top())));
                comments.pop();
                if (++cursor == slots.end()) cursor = slots.begin();
            }
            for (auto it = current_comments.begin() + drawed; it != current_comments.end(); ++it) {
                it->updateState(time, configure);
                configure->draw(it->texture, it->state.x, it->state.y);
            }
        }
};

class CommentManagerImpl : public CommentManager {
    private:
        std::map<CommentType, std::unique_ptr<BaseCommentPool>> pools;

        const CommentManagerConfigure *configure;
    public:
        CommentManagerImpl(const CommentManagerConfigure *configure) : configure(configure) {
            pools.emplace(CommentType::normal,  std::unique_ptr<BaseCommentPool>(new CommentPool<ReverseScorllableCommentObject>));
            pools.emplace(CommentType::reverse, std::unique_ptr<BaseCommentPool>(new CommentPool<ScorllableCommentObject>));
            pools.emplace(CommentType::top,     std::unique_ptr<BaseCommentPool>(new CommentPool<StationaryCommentObject>));
            pools.emplace(CommentType::bottom,  std::unique_ptr<BaseCommentPool>(new CommentPool<ReverseStationaryCommentObject>));
        }

        void pushComment(const CommentBase &comment) {
            auto pool = pools.find(comment.type);
            if (pool != pools.end()) {
                pool->second->pushComment(comment);
            }
        }

        void update(double time) {
            for (const auto &pool : pools)
                pool.second->update(time, configure);
        }
};

CommentManager *makeCommentManager(const CommentManagerConfigure *configure) {
    return new CommentManagerImpl(configure);
}

}
