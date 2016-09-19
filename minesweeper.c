#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define clear() printf("\033[H\033[J")

//color
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


//Board piece
#define MINE 9
#define EMPTY 0


//Declare variables
int p[21][21]={0}; // Store board
int maxrow, maxcol; //Max number of row/col (size of board: 0->maxrow, 0->maxcol)
int minum; //Number of mines
int note[250];



//Print a line of Info area
char print_info(int line){
  switch (line) {
    case 0: printf(ANSI_COLOR_GREEN  "MINESWEEPER TERMINAL VERSION" ANSI_COLOR_RESET); break;
    case 1: printf(ANSI_COLOR_YELLOW "         Coded by VietAnhDev" ANSI_COLOR_RESET); break;
    case 2: printf(ANSI_COLOR_YELLOW "     Website: vietanhdev.com" ANSI_COLOR_RESET); break;  
  }
}



//Print a piece of the board
void print_p(int row, int col){
  switch (p[row][col]) {
    case EMPTY: printf(ANSI_COLOR_GREEN   "◯"   ANSI_COLOR_RESET); break;
    case 1: printf("1"); break;
    case 2: printf(ANSI_COLOR_MAGENTA    "2"    ANSI_COLOR_RESET ); break;
    case 3: printf(ANSI_COLOR_RED    "3"    ANSI_COLOR_RESET ); break;
    case 4: printf(ANSI_COLOR_CYAN    "4"    ANSI_COLOR_RESET ); break;
    case MINE: printf(ANSI_COLOR_YELLOW  "U"  ANSI_COLOR_RESET); break;
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
  time_t t;
  int r;
  int row, col;
  int iminum = 0; //number of mines already put
  srand((unsigned) time(&t));
  while (iminum < minum) {
    r = rand() % ((maxrow + 1) * (maxcol + 1));
    row = (r - 1) / (maxcol + 1);
    col = r - row * (maxrow + 1) - 1;
    if (p[row][col] == EMPTY && mi_count(row, col) <= 4) {
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






int main(int argc, char *argv[]) {

  //init
  maxrow = 18;
  maxcol = 18;
  minum = 50;

  print_board();
  gen_mine();
  gen_num();
  print_board();
  return 0;
}
