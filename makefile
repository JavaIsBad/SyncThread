CFLAGS = -Wall
OPATH = obj/
IFLAGS = -Iinclude/

vpath %.c src/
vpath %.h include/
vpath %.o obj/

exo1 : exo1.o | bin
	gcc $(CFLAGS) -o exo1 $(OPATH)exo1.o -lpthread
	mv $@ bin/
	
exo2 : exo2.o monsem.o | bin
	gcc $(CFLAGS) -o exo2 $(OPATH)exo2.o $(OPATH)monsem.o -lpthread
	mv $@ bin/

exo1.o : exo1.c exo1.h
exo2.o : exo2.c exo1.h monsem.h
monsem.o : monsem.c monsem.h

%.o : | obj
	gcc $(CFLAGS) -c $< $(IFLAGS)
	mv $@ $(OPATH)
	
obj : 
	mkdir obj

bin : 
	mkdir bin

clean : 
	rm obj/* bin/*

