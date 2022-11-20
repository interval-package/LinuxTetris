.PHONY: run clean

build: Tetris
	make run
	make clean

Tetris: main.o Tetris_action.o Tetris_gui.o Tetris_init.o Tetris.h
	gcc Tetris_init.o Tetris_action.o Tetris_gui.o main.o -o Tetris -l curses

main.o: main.c Tetris.h
	gcc -c main.c -o main.o -l curses

Tetris_gui.o: Tetris_gui.c Tetris.h
	gcc -c Tetris_gui.c -l curses

Tetris_action.o: Tetris_action.c Tetris.h
	gcc -c Tetris_action.c -l curses

Tetris_init.o: Tetris_init.c Tetris.h
	gcc -c Tetris_init.c -l curses

run:
	./Tetris -c

clean:
	rm -f *.o Tetris
	clear