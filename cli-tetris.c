#include "cli-tetris.h"

int prev_rotation = 1;
int rotation = 1;

int main(){

	// set random seed
	srand(time(NULL));

	// initialize tetrominos

	Tetromino t = {
        {
            0,0,0,0,
		    0,0,0,0,
		    0,0,1,0,
		    0,1,1,1
	    },
        {1,0,0,2}
    };
	
	Tetromino l = {
        {
            0,0,0,0,
		    0,0,0,1,
		    0,0,0,1,
		    0,0,1,1
	    },
        {2,0,0,1}
    };
	
	Tetromino i = {
        {
            0,0,0,1,
		    0,0,0,1,
		    0,0,0,1,
		    0,0,0,1
	    },
        {3,0,0,0}
    };
	
	Tetromino o = {
        {
            0,0,0,0,
		    0,1,1,0,
		    0,1,1,0,
		    0,0,0,0
	    },
        {1,1,1,1}
    };

	// put them into an array to be able to choose one randomly	
	Tetromino* pieces[] = {&i, &t, &l, &o};
	
	// store 'em in a variable
	Tetromino* piece;

	// initialize them boards
	int board[H][W];

    int t_index = 0;

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
  	  	
	piece = pieces[t_index];
  	
  	while (1) {

	  	clear();
	
		ty++;
		
		int ch = getch();
	  	
		get_input(&tx, &ty, ch);
	  	
	  	move_tetromino(&piece, pieces, board, &tx, &ty, &prev_tx, &prev_ty, &t_index);
		
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

        case KEY_UP:
            prev_rotation = rotation;
            rotation = (rotation <= 3 ? rotation+1 : 0);
            return;
			
		default:
			return;
	
	}
}

// returns false if any of the positions is forbidden
int can_move(Tetromino* piece, int board[H][W], int* tx, signed int* ty){

	if ( ((*tx)+(piece->offset[0+rotation])+1) == 0 || (*tx)+(T-1) == W-1 || (*ty)+(T-1) == H-1) return 0;

	for (int i = 0; i<T; i++){
		for (int j = 0; j<T; j++){
			
			int x = j+(*tx);
			int y = i+(*ty);
			
            int ti = rotate(j, i, rotation);

			int value = (piece->points)[ti];
			
			if (value && board[y][x]) return 0;
		}
	}
	
	return 1;
}

// removes tetrominos old instance from board array
void remove_old(Tetromino* piece, int board[H][W], int tx, signed int ty){
	for (int i = 0; i<T; i++){
		for (int j = 0; j<T; j++){
		
			int x = j+tx;
			signed int y = i+ty;
		
            int ti = rotate(j, i, prev_rotation);
	
			if((piece->points)[ti]) board[y][x] = 0;
		}
	}
    prev_rotation = rotation;
}

// puts tetromino in place
void move_tetromino(Tetromino** piece, Tetromino* pieces[], int board[H][W], int* tx, int* ty, int* prev_tx, signed int* prev_ty, int* t_index){

	remove_old(*piece, board, *prev_tx, *prev_ty);
	
	if (can_move(*piece, board, tx, ty)){
		
		add_tetromino(*piece, board, tx, ty);
		
		*prev_tx = *tx;
		*prev_ty = *ty;
		
	}else{
		
		if (landed(*piece, board, tx, ty)){
		
			// draw it, then reset position values
			add_tetromino(*piece, board, tx, prev_ty);
			
			//choose random tetromino from list
			*t_index = rand() % tn;
			*piece = pieces[(*t_index)];

            // reset values
            rotation = 0;

			*tx = 1;
			*ty = -3;
			
			*prev_tx = 1;
			*prev_ty = -3;
			
		}
		else{
		
			*tx = *prev_tx;
			
			add_tetromino(*piece, board, tx, ty);
			
			*prev_ty = *ty;
			
		}	
	}
}

void add_tetromino(Tetromino* piece, int board[H][W], int* tx, signed int* ty){

	for (int i = 0; i<T; i++){
		for (int j = 0; j<T; j++){
		
			int x = j+(*tx);
			signed int y = i+(*ty);
			
            int ti = rotate(j, i, rotation);

			int value = (piece->points)[ti];
			
            if (value) board[y][x] = value;
			
		}
	}

}

// such ASCII
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
        for (int j = 0; j<T; j++){

            int ti = rotate(j, i, rotation);

            if ( (piece->points)[ti] && board[(*ty)+i+1][(*tx)+j]) return 1;
        }
    }	
	
	return 0;
	
}
			
int rotate(int px, int py, int r){

    switch(r){
        case 0: return (py * 4 + px);
        case 1: return (12 + py - (px * 4) );
        case 2: return ( 15 - (py * 4) - px);
        case 3: return ( 3 + (px * 4) - py );
    }
    return 0;

}
