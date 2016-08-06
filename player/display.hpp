#pragma once

namespace BilibiliConsole {

struct Display {
    void doLock(void **p_pixels);
    void doUnlock(void *const *p_pixels);
    void doDisplay();
    void doLoop();
};

}
