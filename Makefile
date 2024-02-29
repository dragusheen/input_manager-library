##
## EPITECH PROJECT, 2022
## makefile
## File description:
## makefile of project
##

SRC	=	$(wildcard src/*.cpp)	\

OBJ = 	$(SRC:%.cpp=%.o)

LIB	=	libInputManager.a

FLAGS	=	-std=c++20		\
			-Wall			\
			-Wextra			\
			-Werror			\
			-I./include

COMP	=	g++

all:	$(LIB)

$(LIB):	$(OBJ)
	ar rcs $(LIB) $(OBJ)

%.o: %.cpp
	$(COMP) $(FLAGS) -c -o $@ $<

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(LIB)

re: fclean all

.PHONY: all clean fclean re
