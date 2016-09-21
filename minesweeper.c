#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

//Declare variables
int p[21][21]={0}; // Store board
int opened[21][21]={0}; // Store the stage of opening
int flag[21][21]={0}; // Store the flags (question, bomb flag)
int bomb_flag_number = 0; //Number of bomb flag already put
int cs_row = 0, cs_col = 0; // Current position of Cursor
int maxrow, maxcol; //Max number of row/col (size of board: 0->maxrow, 0->maxcol)
int minum; //Number of mines

int end_of_game = 0; // =0 if playing, =1 if game ended.

// Control keys
//directional keys
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define ENTER 4
#define SPACE 5


//Board piece
#define MINE 9
#define EMPTY 0
#define QUESTION_FLAG 99
#define BOMB_FLAG 100
#define NO_FLAG 0


#define clear() printf("\033[H\033[J")

#define RED 31
#define GREEN 32
#define ORANGE 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define YELLOW 38
#define NORMAL 0

void iprint(char str[], int color) {
  switch (color) {
    case RED: printf("\x1b[31m"); printf("%s",str); printf("\033[0m"); break;
    case GREEN: printf("\x1b[32m"); printf("%s",str); printf("\033[0m"); break;
    case ORANGE: printf("\x1b[33m"); printf("%s",str); printf("\033[0m"); break;
    case BLUE: printf("\x1b[34m"); printf("%s",str); printf("\033[0m"); break;
    case MAGENTA: printf("\x1b[35m"); printf("%s",str); printf("\033[0m"); break;
    case CYAN: printf("\x1b[35m"); printf("%s",str); printf("\033[0m"); break;
    case YELLOW: printf("\033[38;5;228m"); printf("%s",str); printf("\033[0m"); break;
    default: printf("%s", str);
  }
}



// Function to read input keys from keyboard
int getch() {
  struct termios oldt,
                 newt;
  int            ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}

// Get color of a position on the board
int get_p_color(int row, int col){
  //If seclected position
  if (cs_row == row && cs_col == col)
  	return YELLOW;
  //If unopened position
  if ( !opened[row][col] )
  	return NORMAL;
  //Other cases
  switch (p[row][col]) {
    case EMPTY:
    	if (opened[row][col])
    		return BLUE;
    	else
    		return NORMAL;
    case 1: return GREEN;
    case 2: return ORANGE;
    case 3: return MAGENTA;
    case 4: return CYAN;
    case MINE: return RED;
    default: return NORMAL;
  }
}



//Print a line of Info area
char print_info(int line){
  switch (line) {
    case 0: iprint("MINESWEEPER TERMINAL VERSION", GREEN); break;
    case 1: iprint("         Coded by VietAnhDev", CYAN); break;
    case 2: iprint("     Website: vietanhdev.com", CYAN); break;
    case 3: iprint("____________________________", GREEN); break;
    case 5: printf("MINES LEFT: %d", minum - bomb_flag_number); break;
    case 7: iprint("    *** HOW TO PLAY? ***", GREEN); break;
    case 8: iprint("← left ↑ up → right ↓ down : move", GREEN); break;
    case 9: iprint("Enter/Space: Open", GREEN); break;
    case 10: iprint("b : bomb flag", GREEN); break;
    case 11: iprint("f : question flag", GREEN); break;
    case 12: iprint("n : start a new game.", GREEN); break;
  }
}



//Print a piece of the board
void print_p(int row, int col){

  int color; // Color of position

  //If this is a selected position, color = yellow.
  color = get_p_color(row, col);

  if (!opened[row][col]) {

  	if (flag[row][col] == QUESTION_FLAG)
	    iprint("?", color);
	else if (flag[row][col] == BOMB_FLAG)
		iprint("x", color);
	else
		iprint("⎔", color);

  } else {
  	 switch (p[row][col]) {
	    case EMPTY:
	    	if (opened[row][col])
	    		iprint("o", color);
			else
				iprint("⎔", color);
	    	break;
	    case MINE: iprint("M", color); break;
	    case 1: iprint("1", color); break;
	    case 2: iprint("2", color); break;
	    case 3: iprint("3", color); break;
	    case 4: iprint("4", color); break;
	  }
  }

}

//Print board (including info)
void print_board(){
  int i,j;
  clear();
  printf("\n");
  for (i = 0; i <= maxrow; i++) {
     printf("  ");
     for (j = 0; j <= maxcol; j++) {
       print_p(i, j); printf(" ");
     }
     printf(" ");
     print_info(i);
     printf("\n");
  }
  printf("\n");
}


// Function to count the number of mines around a position (row, col)
int mi_count(int row, int col){
  int i,j;
  int near_minum = 0;
  for (i = -1; i < 2; i++) {
    for (j = -1; j < 2; j++) {
      if (row + i >= 0 && row + i <= maxrow && col + j >= 0 && col + j <= maxcol){
        if (p[row + i][col + j] == MINE)
        	near_minum += 1;
      }
    }
  }
  return near_minum;
}

// Generate mines on board
void gen_mine(){
  int i, j;
  time_t t;
  int r;
  int row, col;
  int iminum = 0; //number of mines already put
  srand((unsigned) time(&t));
  while (iminum < minum) {
    r = rand() % ((maxrow + 1) * (maxcol + 1));
    row = (r - 1) / (maxcol + 1);
    col = r - row * (maxrow + 1) - 1;

    int should_add = 1; //should add mine to this position? If number of around mines < 4 then we can add more mines here.
    for (i = -1; i < 2; i++) {
	    for (j = -1; j < 2; j++) {
	      if (p[row + i][col + j] != MINE && mi_count(row + i, col + j) > 3) {
	      	should_add = 0;
	      }
	    }
	  }
    if (p[row][col] == EMPTY && should_add == 1) {
      p[row][col] = MINE;
      iminum += 1;
    };
  }
}

// Count and generate number of mines of board
void gen_num(){
  int i,j;
  for (i = 0; i <= maxrow; i++) {
    for (j = 0; j <= maxcol; j++) {
      if (p[i][j] == EMPTY){
        p[i][j] = mi_count(i, j);
      }
    }
  }
}


// Count unopened positions
int unopened_count(){
	int unopened, i, j;
	for (i = 0; i <= maxrow; i++) {
		for (j = 0; j <= maxcol; j++) {
			if (!opened[i][j])
				unopened += 1;
			}
		}
	return unopened;
}

// Check Win
void check_win(int row, int col){ // row, col already opened
	int i, j;
	int won = 0; // =-1: Lose; =1: Won; =0: Default
	if (p[row][col] == MINE) {
		won = -1; // If open a mine -> Lose
	} else if (unopened_count() == minum) {
		won = 1; // If unopened positions = number of mines -> Won
	} else if (bomb_flag_number == minum){ // Number of Bomb flag = Number of Mines
		won = 1;
		for (i = 0; i <= maxrow; i++) {
			for (j = 0; j <= maxcol; j++) {
				if (flag[i][j] == BOMB_FLAG && p[i][j] != MINE) {
					won = 0;
				}
			}
		}
	}

	switch(won) {
		case -1:{ //Lose
			for (i = 0; i <= maxrow; i++) {
				for (j = 0; j <= maxcol; j++) {
					if (p[i][j] == MINE) {
						opened[i][j] = 1;
					}
				}
			}
			print_board();
			iprint("You Lose! Please Start a new game by pressing [n] key...", GREEN);
			end_of_game = 1;
			break;
		}
		case 1:{ //Won
			for (i = 0; i <= maxrow; i++) {
				for (j = 0; j <= maxcol; j++) {
					if (p[i][j] == MINE) {
						opened[i][j] = 1;
					}
				}
			}
			print_board();
			iprint("You Won! Please Start a new game by pressing [n] key...", GREEN);
			end_of_game = 1;
			break;
		}


	}
}


//Open an empty position
void open_empty_pos(int row, int col) {
	int i, j;
	opened[row][col] = 1; //Open a position
	if (p[row][col] == EMPTY) {
		for (i = -1; i < 2; i++) {
		    for (j = -1; j < 2; j++) {
				if (i!=0 || j!=0) { //Not current position
					if (row + i >= 0 && row + i <= maxrow && col + j >=0 && col + j <= maxcol) { // Inside the board
						if (!opened[row + i][col + j])
							open_empty_pos(row + i, col + j);
					}
				}
			}
		}
	}
}

// Open a position on board
int open(int row, int col) {
	if (!opened[row][col]) {
		switch(p[row][col]) {
			case 1:
			case 2:
			case 3:
			case 4: opened[row][col] = 1; break;
			case EMPTY: open_empty_pos(row, col); break;
		}
		print_board();
	}

	// Check Win :D
	check_win(row, col);
}


// Question Flag for the position you doubt that it has a bomb
void question(int row, int col) {
	if (!opened[row][col]) {
		if (flag[row][col] != QUESTION_FLAG)
			flag[row][col] = QUESTION_FLAG;
		else
			flag[row][col] = NO_FLAG;
		print_board();
	}
}


// Bomb Flag for the position you're sure that it has a bomb
void bomb_flag(int row, int col) {
	if (!opened[row][col]) {
		if (flag[row][col] != BOMB_FLAG) {
			flag[row][col] = BOMB_FLAG;
			bomb_flag_number += 1;
		} else {
			flag[row][col] = NO_FLAG;
			bomb_flag_number -= 1;
		}
		print_board();
	}
}


// Start a newgame
int newgame() { //return 0: newgame; return 1: cancel
	int i, j;
	char level;

	do { // Input level
		print_board();
		iprint("Start a new game. Choose Level (1-3). c: cancel: ", GREEN);
		level = getch();
	} while (level!='1' && level!='2' && level!='3' && level!='c');

	switch(level) {
		case 'c': return 1;
		case '1': // Level 1
			{
				maxrow = 12;
				maxcol = 12;
				minum = 20;
				cs_row = 6, cs_col = 6;
				break;
			}
		case '2': // Level 2
			{
				maxrow = 15;
				maxcol = 15;
				minum = 30;
				cs_row = 8, cs_col = 8;
				break;
			}
		case '3': // Level 3
			{
				maxrow = 20;
				maxcol = 20;
				minum = 60;
				cs_row = 10, cs_col = 10;
				break;
			}
	}


	//Reset Variables
	for (i = 0; i <= maxrow; ++i) {
		for (j = 0; j <= maxcol; ++j) {
			p[i][j] = 0;
			opened[i][j] = 0;
			flag[i][j] = 0;
		}
	}

	bomb_flag_number = 0;
	end_of_game = 0;

	gen_mine();
  	gen_num();

  	print_board();

}


// Process input keys
int pressed(int key){
  switch(key) {
  	case UP: if(!end_of_game)
  		{
  			if (cs_row == 1) {
  				cs_row = maxrow;
  			} else {
  				cs_row -= 1;
  			}
  			print_board();
  			break;
  		}
  	case DOWN: if(!end_of_game)
  		{
  			if (cs_row == maxrow) {
  				cs_row = 0;
  			} else {
  				cs_row += 1;
  			}
  			print_board();
  			break;
  		}
  	case LEFT: if(!end_of_game)
  		{
  			if (cs_col == 0) {
  				cs_col = maxcol;
  			} else {
  				cs_col -= 1;
  			}
  			print_board();
  			break;
  		}
  	case RIGHT: if(!end_of_game)
  		{
  			if (cs_col == maxcol) {
  				cs_col = 0;
  			} else {
  				cs_col += 1;
  			}
  			print_board();
  			break;
  		}
  	case SPACE:
  	case ENTER: if(!end_of_game) {
  				open(cs_row, cs_col); break;
  			}
  	case 'f': if(!end_of_game) question(cs_row, cs_col); break;
  	case 'b': if(!end_of_game) bomb_flag(cs_row, cs_col); break;
  	case 'n': newgame(); break;
  }
}


int main(int argc, char *argv[]) {

  //init
  maxrow = 20;
  maxcol = 20;
  minum = 60;
  end_of_game = 0;

  print_board();
  gen_mine();
  gen_num();
  print_board();

  // Main loop
  // Recognize input keys from keyboard
  int ch;
  int exit;
  while (1) {
      ch = getch();
      switch (ch) {
        case '\033':
          	{
	            getch(); //remove [
	            switch(getch()) { // the real value
	              case 'A':
	                pressed(UP);
	                break;
	              case 'B':
	                pressed(DOWN);
	                break;
	              case 'C':
	                pressed(RIGHT);
	                break;
	              case 'D':
	                pressed(LEFT);
	                break;
	            }
	            break;
        	}
        case '\n': pressed(ENTER); break;
        case ' ': pressed(SPACE); break;
        default: pressed(ch);
      }

  }
  return 0;
}