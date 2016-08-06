#pragma once
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <vlc/vlc.h>
#include <exception>
#include <string>
#include "../utils/custom_pointer.hpp"

struct BasicInfo {
    fb_var_screeninfo vinfo;

    BasicInfo() {
        int fbfd = open("/dev/fb0", O_RDWR);
        ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
        if (!vinfo.xres) {
            vinfo.xres = 800;
            vinfo.yres = 600;
        }
        close(fbfd);
    }
};

struct vlc_error : public std::runtime_error {
    vlc_error(const char* m) : std::runtime_error(m) {}
};

//libvlc_instance_t
template<>
libvlc_instance_t *         custom_pointer<libvlc_instance_t>::     Create() {
    char const *vlc_argv[] = {
        "--no-xlib", // Don't use Xlib.
    };
    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
    auto ret = libvlc_new(vlc_argc, vlc_argv);
    if (!ret) {
        std::cerr << "Couldn't init libvlc:" << std::endl;
        throw vlc_error("Couldn't init libvlc");
    }
    return ret;
}
template<>
void                        custom_pointer<libvlc_instance_t>::     Destroy(libvlc_instance_t *t) {
    libvlc_release(t);
}

//libvlc_media_t
template<>
template<>
libvlc_media_t *            custom_pointer<libvlc_media_t>::        Create<custom_pointer<libvlc_instance_t>, std::string>(custom_pointer<libvlc_instance_t> vlc, std::string location) {
    return libvlc_media_new_location(vlc, location.c_str());
}
template<>
void                        custom_pointer<libvlc_media_t>::        Destroy(libvlc_media_t *t) {
    libvlc_media_release(t);
}

//libvlc_media_player_t
template<>
template<>
libvlc_media_player_t *     custom_pointer<libvlc_media_player_t>:: Create<custom_pointer<libvlc_media_t>>(custom_pointer<libvlc_media_t> media) {
    return libvlc_media_player_new_from_media(media);
}
template<>
void                        custom_pointer<libvlc_media_player_t>:: Destroy(libvlc_media_player_t *t) {
    libvlc_media_player_stop(t);
    libvlc_media_player_release(t);
}

