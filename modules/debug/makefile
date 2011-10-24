MODNAME = debug

all:		debug
			cp $(MODNAME).so.1.0 ../

debug:		main.o
			$(CC) -shared -Wl,-soname,$(MODNAME).so.1 -o $(MODNAME).so.1.0 main.o

main.o:		main.cpp
			$(CC) -fPIC -c main.cpp -o main.o
