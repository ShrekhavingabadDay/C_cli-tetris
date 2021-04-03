// TODO: -show where piece will land
//       -game over screen
//       -Q exits the game


#include "cli-tetris.h"

int colors = 1;

int gameover = 0;

int prev_rotation = 0;
int rotation = 0;

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
		    0,0,0,2,
		    0,0,0,2,
		    0,0,2,2
	    },
        {2,0,0,1}
    };
	
	Tetromino i = {
        {
            0,0,0,3,
		    0,0,0,3,
		    0,0,0,3,
		    0,0,0,3
	    },
        {3,0,0,0}
    };
	
	Tetromino o = {
        {
            0,0,0,0,
		    0,4,4,0,
		    0,4,4,0,
		    0,0,0,0
	    },
        {1,1,1,1}
    };

    Tetromino s = {
        {
            0,0,0,0,
            0,0,0,0,
            0,0,5,5,
            0,5,5,0
        },
        {1,0,0,2}
    };

    Tetromino z = {
        {
            0,0,0,0,
            0,0,0,0,
            6,6,0,0,
            0,6,6,0
        },
        {0,0,1,2}
    };

	// put them into an array to be able to choose one randomly	
	Tetromino* pieces[] = {&o, &z, &l, &s, &i, &t};
	
	// store 'em in a variable
	Tetromino* piece;

	// initialize them boards
	int board[H][W];

    // a.k.a. tetromino index
    int t_index = 0;

	for (int i = 0; i<H; i++)
		for (int j = 0; j<W; j++)
			board[i][j] = 0;
	
	// variables for tetrominos position

	int tx = 2; 
	signed int ty = -3;
	
	int prev_tx = 2;
	signed int prev_ty = -3;

 
    int ghost_x = 0;
    int ghost_y = 0;
  
	// initialize ncurses
	initscr();
  	cbreak();
  	noecho();
  	keypad(stdscr, TRUE);
  	curs_set(0);
  	nodelay(stdscr, 1);
  
    //get terminal sizes
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // assign values to positions that will allow us to draw the board in the middle of the screen
    int x_offset = (w.ws_col / 2) - (W / 2);
    int y_offset = (w.ws_row / 2) - (H / 2);
 
    // check for colors, initialize them
    if (!has_colors){
        colors = 0;
    }
    else{
        use_default_colors();
        start_color();
        init_pair(1, COLOR_RED, -1);
        init_pair(2, COLOR_BLUE, -1);
        init_pair(3, COLOR_YELLOW, -1);
        init_pair(4, COLOR_GREEN, -1);
        init_pair(5, COLOR_MAGENTA, -1);
        init_pair(6, COLOR_CYAN, -1);
        init_pair(7, COLOR_WHITE, -1);
    }
  	  	
	piece = pieces[t_index];
    
    // determines if we need the ghost or not
    int need_ghost;

    // main gameloop  	
  	while (!gameover) {

	  	clear();
	
		ty++;
		
		int ch = getch();
	  	
		get_input(&tx, &ty, ch);
	  	
	  	move_tetromino(&piece, pieces, board, &tx, &ty, &prev_tx, &prev_ty, &t_index);
		
		display_board(board, x_offset, y_offset);

        need_ghost = ghost_coords(&ghost_x, &ghost_y, &tx, &ty, piece, board);

        if (need_ghost) draw_ghost(piece, ghost_x, ghost_y, x_offset, y_offset);
	
		refresh();

		usleep(DELAY);
	
	}

    clear();
	
	// restore terminal state
	endwin();
	
	return 0;

}

int got_tetris(int board[H][W]){

    int tetris = 1;

    for (int i=0; i<H; i++){
        for (int j=0; j<W; j++){
            if (board[i][j]==0) {
                tetris = 0;
                break;
            } 
        }
        if (tetris) return 1;
        tetris = 1;
    }
    return 0;

}

void clean_tetris(int board[H][W]){
    int tetris = 1;

    for (int i=0; i<H; i++){
        for (int j=0; j<W; j++){
            if (board[i][j]==0) {
                tetris = 0;
                break;
            } 
        }
        if (tetris) {
            // move the stuff down by one row
            for (int k=0; k<W; k++)
                for (int l=i-1; l>-1; l--)
                    board[l+1][k] = board[l][k];
            
        }

        tetris = 1;

    }
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
            rotation = (rotation <= 2 ? rotation+1 : 0);
            return;
			
		default:
			return;
	
	}
}

// returns false if any of the positions is forbidden
int can_move(Tetromino* piece, int board[H][W], int* tx, signed int* ty){

    // epicest border collision-detection ever
	if ( ((*tx)+1+(piece->offset[rotation])) == 0 || (*tx)+T-2-(piece->offset[r_offset(rotation)]) == W-1 
    || (*ty)+T-1-(piece->offset[bottom_offset(rotation)]) == H-1) return 0;

	for (int i = 0; i<T; i++){
		for (int j = 0; j<T; j++){
			
			int x = j+(*tx);
			int y = i+(*ty);
			
            int ti = rotate(j, i, rotation);

			int value = (piece->points)[ti];
			
			if (value && (board[y][x]>0) ) return 0;
		}
	}
	
	return 1;
}

int can_move_sideways(Tetromino* piece, int board[H][W], int *tx, signed int* ty, int *prev_tx){
 
	if ( ((*tx)+1+(piece->offset[rotation])) == 0 || (*tx)+T-2-(piece->offset[r_offset(rotation)]) == W-1 ) return 0;

	for (int i = 0; i<T; i++){
		for (int j = 0; j<T; j++){
			
            int prev_x = j+(*prev_tx);
			int x = j+(*tx);
			int y = i+(*ty);
			
            int ti = rotate(j, i, rotation);

			int value = (piece->points)[ti];
			
			if (value && (board[y][x]>0)) return 0; //&& !(board[y][prev_x]) ) return 0;
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
    // prev_rotation = rotation;
}

// puts tetromino in place
void move_tetromino(Tetromino** piece, Tetromino* pieces[], int board[H][W], int* tx, int* ty, int* prev_tx, signed int* prev_ty, int* t_index){

    int moved_sideways = ( ((*tx) - (*prev_tx)) != 0 );
    int rotated = (prev_rotation != rotation);

    remove_old(*piece, board, *prev_tx, *prev_ty);
		
	if (can_move(*piece, board, tx, ty)){
        
		add_tetromino(*piece, board, tx, ty);
		
		*prev_tx = *tx;
		*prev_ty = *ty;

        prev_rotation = rotation;
		
	}else{
        
        if (rotated) rotation = prev_rotation;

        if (moved_sideways && !(can_move_sideways(*piece, board, tx, ty, prev_ty)) ) *tx = *prev_tx;
        
		if (landed(*piece, board, tx, ty)){

			// draw it, then reset position values
			add_tetromino(*piece, board, tx, prev_ty);
			
            // check if we reached the top -> if yes, game over
            for (int i=0; i<W; i++)
                if (board[0][i]>0) {gameover = 1; return;}


            // check if we got any tetrises
            if (got_tetris(board)) clean_tetris(board);

			// choose random tetromino from list
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

// sets the coordinates of the ghost and returns one if we can draw it
int ghost_coords(int* x, int* y, int* tx, int* ty, Tetromino* piece, int board[H][W]){

    *x = (*tx);
    *y = (*ty)+T;

    while (!landed(piece, board, x, y) && (*y)<H-T) (*y)++; 

    if ( ((*y)-(*ty)) <= T ) return 0;

    return 1;

}

void draw_ghost(Tetromino* piece, int x, int y, int x_offset, int y_offset){
    for (int i=0; i<T; i++){
        for (int j=0; j<T; j++){
            int ti = rotate(j, i, prev_rotation);
            
            if ((piece->points)[ti]>0) mvaddch(y_offset+y+i, x_offset+x+j, '#');
        }
    }
}

// such ASCII
void display_board(int board[H][W], int x, int y){

	// add top border
	for (int i = 1; i<W+2; i++){
		mvaddch(y, x+i, '-');
	}
	
	for (int i = 0; i<H; i++){
	
		// add left border
		mvaddch(y+i, x, '|');
	
		for (int j=0; j<W; j++){
			
		    char c = board[i][j] ? '0' : ' ';
		
            if (colors)	{
                attron(COLOR_PAIR(board[i][j]));
			    mvaddch(y+i+1, x+j+1, c);
                attroff(COLOR_PAIR(board[i][j]));}
            else mvaddch(y+i+1, x+j+1, c);	
			
		}
		
		// add right border
		mvaddch(y+i, W+1+x, '|');
	}
	
	// add bottom border
	for (int i = 0; i<W+2; i++){
		mvaddch(y+H, x+i, '-');
	}
	
}

int landed(Tetromino* piece, int board[H][W], int* tx, signed int* ty){
	
	if ( (*ty)+T-1-(piece->offset[bottom_offset(rotation)]) == H-1 ) return 1;

    for (int i = 0; i<T; i++){
        for (int j = 0; j<T; j++){

            int ti = rotate(j, i, rotation);

            if ( (piece->points)[ti]>0 && (board[(*ty) + i][(*tx) + j]>0) ) return 1;
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

int r_offset(int r){
    switch (r){
        case 0: return 2;
        case 1: return 3;
        case 2: return 0;
        case 3: return 1;
    }
    return 0;
}

int bottom_offset(int r){
    switch (r){
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 0;
    }
    return 0;
}
