NAME = boef
CC = g++
VPATH = modules/*/makefile
MODULES = null
LIBS = -ldl

include $(VPATH)

all:		$(NAME)
			@echo "#ifndef __COMPILE_DATE_H__" > compile_date.h
			@echo "#define __COMPILE_DATE_H__" >> compile_date.h
			@echo `date` >> compile_date.h
			@echo "#endif" >> compile_date.h

modules:	$(MODULES)

$(NAME):	main.o modules.o
			$(CC) main.o modules.o -o $(NAME) $(LIBS) $(CFLAGS)
			
main.o:		main.cpp main.h
			$(CC) main.cpp -c -o main.o $(CFLAGS)

modules.o:	modules.cpp modules.h
			$(CC) modules.cpp -c -o modules.o -ldl $(CFLAGS)

