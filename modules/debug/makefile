MODNAME = debug
MODULES += $(MODNAME)

debug:		debug.lib
			@echo Descending to debug

debug.lib:	main.o
			$(CC) -shared -Wl,-soname,$(MODNAME).so.1 -o $(MODNAME).so.1.0 main.o
			cp $(MODNAME).so.1.0 ../$(MODNAME).so.1.0

main.o:		main.cpp
			$(CC) -fPIC -c main.cpp -o main.o
