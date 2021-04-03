name=tetris

default: build

debug:
	gcc -o test cli-tetris.c -lncurses && ./test

build:
	gcc -o $(name) cli-tetris.c -lncurses

install:
	sudo cp $(name) /usr/bin/ 

uninstall:
	sudo rm /usr/bin/$(name)
