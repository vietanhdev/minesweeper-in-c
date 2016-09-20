#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>


//Declare variables
int p[21][21]={0}; // Store board
int cs_row = 0, cs_col = 0; // Current position of Cursor
int maxrow, maxcol; //Max number of row/col (size of board: 0->maxrow, 0->maxcol)
int minum; //Number of mines

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

// Get color of a position on the board
int get_p_color(int row, int col){
  switch (p[row][col]) {
    case EMPTY: return NORMAL;
    case 1: return GREEN;
    case 2: return ORANGE;
    case 3: return MAGENTA;
    case 4: return CYAN;
    default: return NORMAL;
  }
}



//Print a line of Info area
char print_info(int line){
  switch (line) {
    case 0: iprint("MINESWEEPER TERMINAL VERSION", GREEN); break;
    case 1: iprint("         Coded by VietAnhDev", CYAN); break;
    case 2: iprint("     Website: vietanhdev.com", CYAN); break;
  }
}



//Print a piece of the board
void print_p(int row, int col){
  int color;
  if (cs_row == row && cs_col == col){
    color = YELLOW;
  } else {
    color = get_p_color(row, col);
  }
  switch (p[row][col]) {
    case EMPTY: iprint("⎔", color); break;
    case MINE: iprint("M", color); break;
    case 1: iprint("1", color); break;
    case 2: iprint("2", color); break;
    case 3: iprint("3", color); break;
    case 4: iprint("4", color); break;
    default: iprint("???????????", color); printf("%d", p[row][col]); break;
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



int mi_count(int row, int col){
  int i,j;
  int near_minum = 0;
  for (i = -1; i < 2; i++) {
    for (j = -1; j < 2; j++) {
      if (row + i >= 0 && row + i <= maxrow && col + j >= 0 && col + j <= maxcol && p[row + i][col + j] == MINE){
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
  int should_add; //should add mine to this position?

  while (iminum < minum) {
    r = rand() % ((maxrow + 1) * (maxcol + 1));
    row = (r - 1) / (maxcol + 1);
    col = r - row * (maxrow + 1) - 1;

    should_add = 1;
    for (i = -1; i < 2; i++) {
	    for (j = -1; j < 2; j++) {
	      if (p[row + i][col + j] != MINE && mi_count(row + i, col + j) > 4) {
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
  for (i = 0; i < maxrow; i++) {
    for (j = 0; j < maxcol; j++) {
      if (p[i][j] == EMPTY){
        p[i][j] = mi_count(i, j);
      }
    }
  }
}


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


int pressed(int key){
  printf("%d", key);
}


int main(int argc, char *argv[]) {

  //init
  maxrow = 18;
  maxcol = 18;
  minum = 50;

  print_board();
  gen_mine();
  gen_num();
  print_board();

  //Main loop
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
      }
  }
  return 0;
}
