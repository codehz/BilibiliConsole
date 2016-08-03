SDL_VERSION=2.0.4
AM_CXXFLAGS = -Wall -O2 --std=c++14 $(shell sdl2-config --cflags) $(shell pkg-config libvlc --cflags)
AM_LIBS = $(shell sdl2-config --libs) $(shell pkg-config libvlc --libs)

noinst_LIBRARIES = libplayer.a
libplayer_a_SOURCES = player.cpp