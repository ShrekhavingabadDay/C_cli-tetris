name=tetris

default: build

build:
	gcc -o $(name) cli-tetris.c -lncurses

install:
	sudo cp $(name) /usr/bin/ 

uninstall:
	sudo rm /usr/bin/$(name)
