#include "../comment/comment.h"
#include "../comment/comment_parser.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sys/stat.h>
#include <limits.h>
#include <libgen.h>
#include <unistd.h>

#define ONEMASK ((size_t)(-1) / 0xFF)

static size_t
cp_strlen_utf8(const char * _s)
{
	const char * s;
	size_t count = 0;
	size_t u;
	unsigned char b;

	/* Handle any initial misaligned bytes. */
	for (s = _s; (uintptr_t)(s) & (sizeof(size_t) - 1); s++) {
		b = *s;

		/* Exit if we hit a zero byte. */
		if (b == '\0')
			goto done;

		/* Is this byte NOT the first byte of a character? */
		count += (b >> 7) & ((~b) >> 6);
	}

	/* Handle complete blocks. */
	for (; ; s += sizeof(size_t)) {
		/* Prefetch 256 bytes ahead. */
		__builtin_prefetch(&s[256], 0, 0);

		/* Grab 4 or 8 bytes of UTF-8 data. */
		u = *(size_t *)(s);

		/* Exit the loop if there are any zero bytes. */
		if ((u - ONEMASK) & (~u) & (ONEMASK * 0x80))
			break;

		/* Count bytes which are NOT the first byte of a character. */
		u = ((u & (ONEMASK * 0x80)) >> 7) & ((~u) >> 6);
		count += (u * ONEMASK) >> ((sizeof(size_t) - 1) * 8);
	}

	/* Take care of any left-over bytes. */
	for (; ; s++) {
		b = *s;

		/* Exit if we hit a zero byte. */
		if (b == '\0')
			break;

		/* Is this byte NOT the first byte of a character? */
		count += (b >> 7) & ((~b) >> 6);
	}

done:
	return ((s - _s) - count);
}

Bilibili::CommentManagerConfigure configure = {
    .maxline = 30,
    .height = 30,
    .padding = 2,
    .durations = 6.0f,
    .viewport_width = 800,
    .viewport_height = 600,
    .requestTexture = [](const Bilibili::CommentBase &comment) -> Bilibili::BasicTexture {
        char *cp = new char[comment.content.size() + 1];
        strcpy(cp, comment.content.c_str());
        return {(void *)cp, cp_strlen_utf8(cp) * 8, 16};
    },
    .freeTexture = [](const Bilibili::BasicTexture &texture) {
        free(texture.texture);
    },
    .draw = [](const Bilibili::BasicTexture texture, const float x, const float y) {
        std::cout << "x: " << std::setw(8)  << (int32_t)x << ", y: " << std::setw(8) << (int32_t)y << " content:" << texture.texture << ":" << (char *)texture.texture << std::endl;
    }
};

std::string getDir() {
    char dest[PATH_MAX];
    if (readlink("/proc/self/exe", dest, PATH_MAX) != -1)
        return std::string(dirname(dest));
    throw "Cannot getcwd";
}

int main() {
    Bilibili::CommentManager *cm = Bilibili::makeCommentManager(configure);
    auto cbs = Bilibili::comment_parse(getDir() + "/res/test.xml");
    while (!cbs.empty()) {
        cm->pushComment(cbs.front());
        cbs.pop();
    }
    for (auto i = 0.0f; i < 220; i+=0.02) {
        std::cout << "\033[H\033[Jtime: " << i << std::endl;
        cm->update(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    delete cm;
}
