#CFLAGS =
#CPPFLAGS =
#CXXFLAGS =
AM_CXXFLAGS = -Wall -O3 -fexpensive-optimizations \
           -finline-functions -foptimize-register-move \
           -funroll-loops -fomit-frame-pointer \
           -march=native -fforce-addr -frerun-cse-after-loop \
           -frerun-loop-opt -falign-functions=4
#CXXFLAGS = -O3 -fomit-frame-pointer
#CXXFLAGS = -S -g
#AM_CXXFLAGS = -pg -g

AM_CPPFLAGS = -I/home/thi/projects/shared/include
LIBS =
AM_LDFLAGS = -s -fwhole-program

bin_PROGRAMS = panel_boxes filter_panels make_mosaic

panel_boxes_SOURCES = panel_boxes.cpp
panel_boxes_LDFLAGS = -L/usr/X11R6/lib -lm -lpthread -lX11

filter_panels_SOURCES = filter_panels.cpp
filter_panels_LDFLAGS = -L/usr/X11R6/lib -lm -lpthread -lX11

make_mosaic_SOURCES = make_mosaic.cpp Bitmap.cpp
make_mosaic_LDFLAGS = -L/usr/X11R6/lib -lm -lpthread -lX11
