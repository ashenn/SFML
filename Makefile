CC	=	g++ --std=c++11 -g -Wall
NAME	=	mario
SRC	=	main.cpp                  \
                base/basic.c              \
                base/libList.c            \
                base/libParse.c           \
                base/logger.cpp           \
                base/math.c               \
                base/file.c               \
                base/lib/jsmn/jsmn.c      \
                base/json.c               \
                core/abstractClass.cpp    \
                core/project/project.cpp  \
                core/render/render.cpp        \
                core/object/object.cpp    \
                

OBJ     =       $(SRC:%.c=%.o)
OBJ2	=	$(OBJ:%.cpp=%.o)

RM	=	rm -f
LIB	=	-lsfml-graphics                   \
                -lsfml-window -lsfml-system       \
                -lpthread                         \
                -fsanitize=address                \
                -D_REENTRANT

$(NAME):	$(OBJ2)
		$(CC) $(OBJ2) -L../libs $(LIB) -o $(NAME)

all:		$(NAME)

clean:
		$(RM) $(OBJ2)

fclean:		clean
		$(RM) $(NAME)

re: fclean all