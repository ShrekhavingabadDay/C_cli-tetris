#include "cli-tetris.h"

int main(){

	// set random seed
	srand(time(NULL));

	// initialize tetrominos

	Tetromino t = {
		{0,0,0},
		{0,1,0},
		{1,1,1}
	};
				
	Tetromino i = {
		{0,1,0},
		{0,1,0},
		{0,1,0}
	};
	
	Tetromino l = {
		{0,0,1},
		{0,0,1},
		{0,1,1}
	};
	
	Tetromino o = {
		{0,0,0},
		{0,1,1},
		{0,1,1}
	};
	
	// put them into an array to be able to choose one randomly	
	Tetromino* pieces[] = {&t, &l, &i, &o};
	
	// store 'em in a variable
	Tetromino* piece;

	// initialize them boards
	int board[H][W];

	for (int i = 0; i<H; i++)
		for (int j = 0; j<W; j++)
			board[i][j] = 0;
	
	// variables for tetrominos position
	int tx = 2; 
	signed int ty = -3;
	
	int prev_tx = 2;
	signed int prev_ty = -3;

	// initialize ncurses
	initscr();
  	cbreak();
  	noecho();
  	keypad(stdscr, TRUE);
  	curs_set(0);
  	nodelay(stdscr, 1);
  	  	
	piece = pieces[1];
  	
  	while (1) {
  	
	  	clear();
		
		ty++;
		
		int ch = getch();
	  	
		get_input(&tx, &ty, ch);
	  	
	  	move_tetromino(piece, pieces, board, &tx, &ty, &prev_tx, &prev_ty);
		
		display_board(board);
		
		refresh();
		
		usleep(DELAY);
	
	}
	
	// restore terminal state
	endwin();
	
	return 0;

}

// get input
void get_input(int* tx, int* ty, int ch){

	switch (ch){
	
		case KEY_LEFT:
			(*tx)--;
			return;
		
		case KEY_RIGHT:
			(*tx)++;
			return;
		
		case KEY_DOWN:
			(*ty)++;
			return;
			
		default:
			return;
	
	}
}

// returns false if any of the positions is forbidden
int can_move(Tetromino* piece, int board[H][W], int* tx, signed int* ty){

	if ((*tx) == 0 || (*tx)+(T-1) == W-1 || (*ty)+(T-1) == H-1) return 0;

	for (int i = 0; i<T; i++){
		for (int j = 0; j<T; j++){
			
			int x = j+(*tx);
			int y = i+(*ty);
			
			int value = (*piece)[i][j];
			
			if (value && board[y][x]) return 0;
		}
	}
	
	return 1;
}

// removes tetrominos old instance from board array
void remove_old(int board[H][W], int tx, signed int ty){
	for (int i = 0; i<T; i++){
		for (int j = 0; j<T; j++){
		
			int x = j+tx;
			signed int y = i+ty;
			
			if(board[y][x]) board[y][x] = 0;
		}
	}
}

// puts tetromino in place
void move_tetromino(Tetromino* piece, Tetromino* pieces[], int board[H][W], int* tx, int* ty, int* prev_tx, signed int* prev_ty){

	remove_old(board, *prev_tx, *prev_ty);
	
	if (can_move(piece, board, tx, ty)){
		
		add_tetromino(piece, board, tx, ty);
		
		*prev_tx = *tx;
		*prev_ty = *ty;
		
	}else{
		
		if (landed(piece, board, tx, ty)){
		
			// draw it, then reset position values
			add_tetromino(piece, board, tx, prev_ty);
			
			//choose random tetromino from list
			int r = rand() % 3;
			
			piece = pieces[r];
			
			*tx = 1;
			*ty = -3;
			
			*prev_tx = 1;
			*prev_ty = -3;
			
		}
		else{
		
			*tx = *prev_tx;
			
			add_tetromino(piece, board, tx, ty);
			
			*prev_ty = *ty;
			
		}	
	}
}

void add_tetromino(Tetromino* piece, int board[H][W], int* tx, signed int* ty){

	for (int i = 0; i<T; i++){
		for (int j = 0; j<T; j++){
		
			int x = j+(*tx);
			signed int y = i+(*ty);
			
			int value = (*piece)[i][j];
			
			board[y][x] = value;
			
		}
	}

}

// Hmmmmmmmmmmmm such ASCII
void display_board(int board[H][W]){

	// add top border
	for (int i = 1; i<W+1; i++){
		mvaddch(0, i, '-');
	}
	
	for (int i = 0; i<H; i++){
	
		// add left border
		mvaddch(i, 0, '|');
	
		for (int j=0; j<W; j++){
			
			char c = board[i][j] ? '#' : ' ';
			
			mvaddch(i+1, j+1, c);	
			
		}
		
		// add right border
		mvaddch(i, W, '|');
	}
	
	// add bottom border
	for (int i = 0; i<W+1; i++){
		mvaddch(H, i, '-');
	}
	
}

int landed(Tetromino* piece, int board[H][W], int* tx, signed int* ty){
	
	if ( ((*ty)+2) == (H-1) ) return 1;
	
	for (int i = 0; i<T; i++){
		if (board[2+(*ty)][(*tx)+i]) return 1;
	}
	
	return 0;
	
}
			

