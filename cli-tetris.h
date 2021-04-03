#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define DELAY 100000

#define W 10
#define H 18
#define T 4
#define tn 6

typedef struct Tetromino{

    int points[T*T*sizeof(int)];
    int offset[2*sizeof(int)];

} Tetromino;

void display_board(int board[H][W], int x, int y);

void draw_ghost(Tetromino* piece, int x, int y, int x_offset, int y_offset);

void move_tetromino(Tetromino** t, Tetromino* pieces[], int board[H][W], int* x, signed int* y, int* prev_tx, signed int* prev_ty, int* t_index);

void get_input(int* tx, int* ty, int ch);

void add_tetromino(Tetromino* piece, int board[H][W], int* tx, signed int* ty);

void remove_old(Tetromino* piece, int board[H][W], int tx, signed int ty);

void clean_tetris(int board[H][W]);

int ghost_coords(int* x, int* y, int* tx, int* ty, Tetromino* piece, int board[H][W]);

int is_reserved(int board[H][W], int x, int y, int v);

int can_move(Tetromino* piece, int board[H][W], int* tx, int* ty);

int can_move_sideways(Tetromino* piece, int board[H][W], int* tx, int* ty, int* prev_tx);

int landed(Tetromino* piece, int board[H][W], int* tx, signed int* ty);

int rotate(int px, int py, int r);

int r_offset(int r);

int bottom_offset(int r);

int got_tetris(int board[H][W]);
