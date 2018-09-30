#ifndef COMMON_H
#define COMMON_H

#define LOG_MAIN	1
#define LOG_JSON	1 << 1
#define LOG_PROJECT 1 << 2
#define LOG_OBJ		1 << 3
#define LOG_RENDER	1 << 4
#define LOG_ANIM	1 << 5

#define FPS 60
#define FRAME (1.0 / FPS) * 1000000



#include <SFML/Graphics.hpp>
#include <X11/Xlib.h>
#include "base/logger.h"

#endif
