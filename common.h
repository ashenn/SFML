#ifndef COMMON_H
#define COMMON_H

#define LOG_MAIN		1
#define LOG_JSON		1 << 1
#define LOG_PROJECT 	1 << 2
#define LOG_OBJ			1 << 3
#define LOG_RENDER		1 << 4
#define LOG_ANIM		1 << 5
#define LOG_EVENT		1 << 6
#define LOG_ASSET		1 << 7
#define LOG_SPRITE		1 << 8
#define LOG_SPRITE_OBJ	1 << 9
#define LOG_SPRITE_ANIM	1 << 10
#define LOG_EVENT_KEY	1 << 11
#define LOG_CHAR		1 << 12
#define LOG_CTRL		1 << 13
#define LOG_CTRL_PLAYER	1 << 14
#define LOG_MOVE		1 << 15
#define LOG_VIEW		1 << 16
#define LOG_COL			1 << 17
#define LOG_COL_LOOP	1 << 18
#define LOG_MUL_TEXT	1 << 19
#define LOG_ENV_OBJ		1 << 20
#define LOG_LEVEL		1 << 21
#define LOG_AI			1 << 22

#define FPS 60
#define FRAME (1.0 / FPS) * 1000000

#define VIEW_DISTANCE 300
#define RENDER_DISTANCE VIEW_DISTANCE


#include <SFML/Graphics.hpp>
#include <X11/Xlib.h>
#include "base/logger.h"
#include "core/exception/exception.h"


#endif
