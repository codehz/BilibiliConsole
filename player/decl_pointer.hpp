#pragma once
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <SDL.h>
#include <SDL_mutex.h>
#include <vlc/vlc.h>
#include "../utils/custom_pointer.hpp"

struct BasicInfo {
    fb_var_screeninfo vinfo;

    BasicInfo() {
        int fbfd = open("/dev/fb0", O_RDWR);
        ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
        close(fbfd);
    }
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
        std::cerr << "Couldn't init libvlc:" << SDL_GetError();
        throw "Couldn't init libvlc";
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

//SDL_Window
template<>
template<>
SDL_Window *                custom_pointer<SDL_Window>::            Create<BasicInfo>(BasicInfo basicInfo) {
    auto ret = SDL_CreateWindow("BiliBiliConsole Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, basicInfo.vinfo.xres, basicInfo.vinfo.yres, SDL_WINDOW_SHOWN);
    if (!ret) {
        std::cerr << "Couldn't create window:" << SDL_GetError();
        throw "Couldn't create window";
    }
    return ret;
}
template<>
void                        custom_pointer<SDL_Window>::            Destroy(SDL_Window *t) {
    SDL_DestroyWindow(t);
}

//SDL_Renderer
template<>
template<>
SDL_Renderer *              custom_pointer<SDL_Renderer>::          Create<custom_pointer<SDL_Window>>(custom_pointer<SDL_Window> window) {
    auto ret = SDL_CreateRenderer(window, -1, 0);
    if (!ret) {
        std::cerr << "Couldn't create renderer:" << SDL_GetError();
        throw "Couldn't create renderer";
    }
    return ret;
}
template<>
void                        custom_pointer<SDL_Renderer>::          Destroy(SDL_Renderer *t) {
    SDL_DestroyRenderer(t);
}

//SDL_Texture
template<>
template<>
SDL_Texture *               custom_pointer<SDL_Texture>::           Create<BasicInfo, custom_pointer<SDL_Renderer>>(BasicInfo basicInfo, custom_pointer<SDL_Renderer> renderer) {
    auto ret = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR565, SDL_TEXTUREACCESS_STREAMING, basicInfo.vinfo.xres, basicInfo.vinfo.yres);
    if (!ret) {
        std::cerr << "Couldn't create texture:" << SDL_GetError();
        throw "Couldn't create texture";
    }
    return ret;
}
template<>
void                        custom_pointer<SDL_Texture>::           Destroy(SDL_Texture *t) {
    SDL_DestroyTexture(t);
}

//SDL_mutex
template<>
SDL_mutex *                 custom_pointer<SDL_mutex>::             Create() {
    return SDL_CreateMutex();
}
template<>
void                        custom_pointer<SDL_mutex>::             Destroy(SDL_mutex *t) {
    SDL_DestroyMutex(t);
}
