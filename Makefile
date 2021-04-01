default: build

build:
	gcc -o tetris cli-tetris.c -lncurses

build-test:
	gcc -o test cli-tetris.c -lncurses

clean:
	rm tetris
