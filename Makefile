CC      =       g++

CFLAGS      = --std=c++11 -g -Wall
CPPFLAGS    = --std=c++11 -g -Wall

NAME	=	mario

MAIN    =       main.cpp

CSRC	=	base/basic.c              \
                base/libList.c            \
                base/libParse.c           \
                base/math.c               \
                base/file.c               \
                base/lib/jsmn/jsmn.c      \
                base/json.c               \

CPPSRC =        base/logger.cpp                 \
                core/exception/exception.cpp    \
                core/abstractClass.cpp          \
                core/asset/asset.cpp                                \
                core/asset/multiTexture/multiTexture.cpp            \
                core/event/event.cpp                                \
                core/event/key/keyEvent.cpp      \
                core/event/eventMgr.cpp          \
                core/project/project.cpp         \
                core/time/timeMgr.cpp            \
                core/render/render.cpp                            \
                core/object/object.cpp                            \
                core/movement/movement.cpp                        \
                core/object/envirement/envirement.cpp             \
                core/object/sprite/spriteObject.cpp               \
                core/object/sprite/character/charObject.cpp       \
                core/character/character.cpp                      \
                core/controller/controller.cpp                    \
                core/controller/player/playerCtrl.cpp             \
                core/animation/animation.cpp                      \
                core/animation/sprite/spriteAnim.cpp      \
                core/view/view.cpp                        \
                core/collision/collision.cpp              \
                core/collision/collisionMgr.cpp           \
                core/control/control.cpp                  \
                


OBJ	=	$(MAIN:%.cpp=%.o)
OBJ1    =       $(CSRC:%.c=%.o)
OBJ2    =       $(CPPSRC:%.cpp=%.o)

RM	=	rm -f
LIB	=	-lsfml-graphics                   \
                -lsfml-window -lsfml-system       \
                -lX11                             \
                -fsanitize=address                \
                -D_REENTRANT


$(NAME):	$(OBJ) $(OBJ1) $(OBJ2)
		$(CC) $(OBJ) $(OBJ1) $(OBJ2) -L../libs $(LIB) -o $(NAME)

all:		$(NAME)

clean:
		$(RM) $(OBJ) $(OBJ1) $(OBJ2)

fclean:		clean
		$(RM) $(NAME)

re: fclean all