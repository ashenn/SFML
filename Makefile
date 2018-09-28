CC	=	g++ --std=c++11 -g
NAME	=	mario
SRC	=	main.c                  \
                base/basic.c            \
                base/libList.c          \
                base/libParse.c         \
                base/logger.c           \
                base/math.c             \
                

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

re: fclean all