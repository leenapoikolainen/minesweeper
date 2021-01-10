#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "minesweeper.h"

using namespace std;

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

void print_move_result(MoveResult result, char revealed[9][9]) {
  switch (result) {
  case INVALID_MOVE:
    cout << "Invalid move." << endl;
    return;
  case REDUNDANT_MOVE:
    cout << "Square is already uncovered or flagged." << endl;
    return;
  case BLOWN_UP:
    cout << "You uncovered a mine and have been blown up!" << endl;
    return;
  case SOLVED_BOARD:
    cout << "You have solved the board, well done!" << endl;
    break;
  case VALID_MOVE:
    cout << "Valid move." << endl;
    break;
  }
  display_board(revealed);
}

/* pre-supplied function to load mine positions from a file */
void load_board(const char *filename, char board[9][9]) {

  cout << "Loading board from file '" << filename << "'... ";

  ifstream in(filename);
  if (!in)
    cout << "Failed!" << endl;
  assert(in);

  char buffer[512];

  int row = 0;
  in.getline(buffer,512);
  while (in && row < 9) {
    for (int n=0; n<9; n++) {
      assert(buffer[n] == '.' || buffer[n] == '*' || buffer[n] == ' ' || buffer[n] == '?' || isdigit(buffer[n]));
      board[row][n] = buffer[n];
    }
    row++;
    in.getline(buffer,512);
  }

  cout << ((row == 9) ? "Success!" : "Failed!") << endl;
  assert(row == 9);
}

/* internal helper function */
void print_row(const char *data, int row) {
  cout << (char) ('A' + row) << "|";
  for (int i=0; i<9; i++) 
    cout << ( (data[i]=='.') ? ' ' : data[i]);
  cout << "|" << endl;
}

/* pre-supplied function to display a minesweeper board */
void display_board(const char board[9][9]) {
  cout << "  ";
  for (int r=0; r<9; r++) 
    cout << (char) ('1'+r);
  cout <<  endl;
  cout << " +---------+" << endl;
  for (int r=0; r<9; r++) 
    print_row(board[r],r);
  cout << " +---------+" << endl;
}

/* pre-supplied function to initialise playing board */ 
void initialise_board(char board[9][9]) {
  for (int r=0; r<9; r++)
    for (int c=0; c<9; c++)
      board[r][c] = '?';
}

/* add your functions here */

bool is_complete(const char mines[9][9], const char revealed[9][9]) {
  // Go through both boards. 
  // If any non-mine square is '?' return false
  for (int row = 0; row < 9; row++){
    for(int column = 0; column < 9; column++){
      if(mines[row][column] == '.' && revealed[row][column] == '?')
        return false;
    }
  }
  return true;
}

int count_mines(const char *position, const char mines[9][9]) {
  // read through the position
  int row = position[0] - 'A';
  int column = position[1] - '1';

  // Initialise mines
  int mines_around = 0;
  
  // Store all possible positions 
  Position positions[8] = {
    Position(-1,-1), Position(-1,0), Position(-1,1),
    Position(0, -1), Position (0, 1),
    Position(1, -1), Position(1, 0), Position(1,1)};
  
  // Go through the positions
  for (int i = 0; i < 8; i++) {
    int test_r = row + positions[i].row;
    int test_c = column + positions[i].column;

    if (test_r > 8 || test_r < 0 || test_c > 8 || test_c < 0) 
      continue;
    if (mines[test_r][test_c] == '*')
      mines_around++;
  }
  return mines_around;
}

// Function that reveals squares aroung a given square
void reveal(const char *position, const char mines[9][9], char board[9][9]){
  int count = count_mines(position, mines);
  int row = position[0] - 'A';
  int column = position[1] - '1';

  // If there are mines around, add the number to the square
  if (count != 0) {
    board[row][column] = count + '0';
    return;
  }
  else {
    // Set the square empty
    board[row][column] = ' ';
    // Check all surrounding squares
    Position positions[8] = {
    Position(-1,-1), Position(-1,0), Position(-1,1),
    Position(0, -1), Position (0, 1),
    Position(1, -1), Position(1, 0), Position(1,1)};

    for (int i = 0; i < 8; i++) {
      int test_r = row + positions[i].row;
      int test_c = column + positions[i].column;

      // if not on the board, continue
      if (test_r > 8 || test_r < 0 || test_c > 8 || test_c < 0) 
        continue;
      // if not uncovered, continue
      if (board[test_r][test_c] != '?')
        continue;
      // if the square is empty on the mines board
      // call the function again with that position
      if (mines[test_r][test_c] == '.') {
        char new_position[2];
        new_position[0] = test_r + 'A';
        new_position[1] = test_c + '1';
        reveal(new_position, mines, board);
      }
    }
  }
}

MoveResult make_move(const char *position, const char mines[9][9], char revealed[9][9]) {
  int length = strlen(position);
  int row = position[0] - 'A';
  int column = position[1] - '1';

  // case invalid move
  if (row > 8 || row < 0 || column > 8 || column < 0)
    return INVALID_MOVE;
  if (length == 3 && position[2] != '*')
    return INVALID_MOVE;

  // case redundant move
  if (revealed[row][column] != '?')
    return REDUNDANT_MOVE;

  // Case mine explodes
  if (length == 2 && mines[row][column] == '*')
    return BLOWN_UP;

  // If none of the above, make the move
  if (length == 3)
    revealed[row][column] = '*';
  else {
    reveal(position, mines, revealed);
  }
  if (!is_complete(mines, revealed))
    return VALID_MOVE;
  else
  {
      return SOLVED_BOARD;
  } 
}

bool is_safe(int row, int column, const char revealed[9][9]){
  // Check all surroundings
  Position positions[8] = {
    Position(-1,-1), Position(-1,0), Position(-1,1),
    Position(0, -1), Position (0, 1),
    Position(1, -1), Position(1, 0), Position(1,1)};
  
  for (int i = 0; i < 8; i++) {
    int test_r = row + positions[i].row;
    int test_c = column + positions[i].column;

    // if not on the board, continue
    if (test_r > 8 || test_r < 0 || test_c > 8 || test_c < 0) {
      continue;
    }
    // If it is digit, check how many mines aroung 
    if (isdigit(revealed[test_r][test_c])) {
      int number = revealed[test_r][test_c] - '0';
      int mines_around = char_around('*', test_r, test_c, revealed);
      // If revealed mines aroung is smaller than the number, return false
      if (mines_around < number) {
        return false;
      }
    }
   
    
  }
  return true;
}

bool is_mine(int row, int column, const char revealed[9][9]){
  // Check all surroundings
  Position positions[8] = {
    Position(-1,-1), Position(-1,0), Position(-1,1),
    Position(0, -1), Position (0, 1),
    Position(1, -1), Position(1, 0), Position(1,1)};
  
  for (int i = 0; i < 8; i++) {
    int test_r = row + positions[i].row;
    int test_c = column + positions[i].column;

    // if not on the board, continue
    if (test_r > 8 || test_r < 0 || test_c > 8 || test_c < 0) 
      continue;

    if (isdigit(revealed[test_r][test_c])){
      int number = revealed[test_r][test_c] - '0';
      int mines_around = char_around('*', test_r, test_c, revealed);
      int uncovered_around = char_around('?', test_r, test_c, revealed);
      //if (number - mines_around == 1 && uncovered_around == 1)
      if (uncovered_around == number - mines_around)
        return true;
    }
  }
  return false;
}

bool find_safe_move(const char revealed[9][9], char *move) {
  // Empty move string
  strcpy(move, "");
  // Go through all uncovered cells
  for (int row = 0; row < 9; row++) {
    for (int column = 0; column < 9; column++) {
      if (revealed[row][column] == '?') {
        if (is_mine(row, column, revealed)){
          *move++ = row + 'A';
          *move++ = column + '1';
          *move++ = '*';
          *move = '\0';
          return true;
        }
        if(is_safe(row, column, revealed)) {
          *move++ = row + 'A';
          *move++ = column + '1';
          *move = '\0';
          return true;
        }
      }
    }
  }
  return false;
}

void convert_to_string(int row, int column, char *position){
  position[0] = row + 'A';
  position[1] = column + '1';
}

int char_around(char ch, int row, int column, const char board[9][9]){
  int count = 0;
  
  // Store all possible positions 
  Position positions[8] = {
    Position(-1,-1), Position(-1,0), Position(-1,1),
    Position(0, -1), Position (0, 1),
    Position(1, -1), Position(1, 0), Position(1,1)};
  
  // Go through the positions
  for (int i = 0; i < 8; i++) {
    int test_r = row + positions[i].row;
    int test_c = column + positions[i].column;

    if (test_r > 8 || test_r < 0 || test_c > 8 || test_c < 0) 
      continue;
    if (board[test_r][test_c] == ch)
      count++;
  }
  return count;
}