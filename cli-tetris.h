#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define DELAY 100000

#define W 10
#define H 18
#define T 3

typedef int Tetromino[T*sizeof(int)][T*sizeof(int)];

void display_board(int board[H][W]);

void move_tetromino(Tetromino* t, Tetromino* pieces[], int board[H][W], int* x, signed int* y, int* prev_tx, signed int* prev_ty);

void get_input(int* tx, int* ty, int ch);

void add_tetromino(Tetromino* piece, int board[H][W], int* tx, signed int* ty);

void remove_old(int board[H][W], int tx, signed int ty);

int is_reserved(int board[H][W], int x, int y, int v);

int can_move(Tetromino* piece, int board[H][W], int* tx, int* ty);

int landed(Tetromino* piece, int board[H][W], int* tx, signed int* ty);

