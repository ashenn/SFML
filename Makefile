CC	=	g++ --std=c++11 -g -Wall
NAME	=	mario

BASE    =       base/basic.c            \
                base/libList.c          \
                base/libParse.c         \
                base/logger.c           \
                base/math.c             \
                base/file.c             \
                base/lib/jsmn/jsmn.c    \
                base/json.c

SRC     =       main.c                  \
                $(BASE)

LIST	=	example/list.c            \
                $(BASE)

OBJ	=	$(SRC:%.c=%.o)

RM	=	rm -f
LIB	=	-lpthread               \
                -fsanitize=address       \
                -D_REENTRANT

$(NAME):	$(OBJ)
		$(CC) $(OBJ) -L../libs $(LIB) -o $(NAME)

all:		$(NAME)

clean:
		$(RM) $(OBJ)

fclean:		clean
		$(RM) $(NAME)

re:             fclean all

list:           $(CC) $(LIST) -L../libs $(LIB) -o list_example