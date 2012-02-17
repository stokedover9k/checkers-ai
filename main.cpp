#include <iostream>
#include <fstream>
#include "checkers-game.h"
#include "player.h"
#include "player-human.h"
#include "player-first.h"
#include "player-minimax.h"
#include "checkers-eval.h"
#include <string>
#include <stdlib.h>
#include <string.h>

#define MAX_TURNS 500

using namespace std;

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
  Checkers game;

  //Player *p1 = new First_Player(string("Sisyphus"), IS_RED);
  Player *p1 = new Minimax_Player(string("Sisyphus"), EvalState::defense, 
				  5, IS_RED);
  Player *p2 = new Minimax_Player(string("Stoked"), EvalState::count_pieces, 
				  5, IS_WHITE);

  const int r = RED;
  const int R = RED_KING;
  const int w = WHITE;
  const int W = WHITE_KING;

  int s[BOARD_HEIGHT * BOARD_WIDTH] = {
    0, r, 0, r, 0, r, 0, r,
    r, 0, r, 0, r, 0, r, 0,
    0, r, 0, r, 0, r, 0, r,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    w, 0, w, 0, w, 0, w, 0,
    0, w, 0, w, 0, w, 0, w,
    w, 0, w, 0, w, 0, w, 0,
  };    //*/
    
  /*  Read in the board state */
  if( argc > 1 ) {
    int *state_ptr = s;
    ifstream board_in(argv[1], ifstream::in);
    
    char c;
    for( int i=BOARD_WIDTH*BOARD_HEIGHT; i>0 && board_in.good(); i-- ) {
      c = (char)board_in.get();
      if( !c ) break;

      switch( c )
	{
	case '\n':  i++; break;
	case 'r':   *(state_ptr++) = RED;          break;
	case 'R':   *(state_ptr++) = RED_KING;     break;
	case 'w':   *(state_ptr++) = WHITE;        break;
	case 'b':   *(state_ptr++) = WHITE;        break;
	case 'W':   *(state_ptr++) = WHITE_KING;   break;
	case 'B':   *(state_ptr++) = WHITE_KING;   break;
	case '.':   *(state_ptr++) = EMPTY;        break;
	default:
	  cout << "ERROR: Invalid character encountered while reading the board state: " << c << endl;
	  exit(1);
	}
    }
    board_in.close();
  }
  
  Board b(s, BOARD_WIDTH * BOARD_HEIGHT);

  /*  Read in beginning side (color) */
  int starting_turn = 0;
  if( argc > 2 ) {
    if( strcmp( argv[2], "r" ) == 0 || strcmp( argv[2], "red" ) == 0 ) {
      starting_turn = 0;
    }
    else if( strcmp( argv[2], "w" ) == 0 || strcmp( argv[2], "white" ) == 0 ) {
      starting_turn = 1;
    }
    else {
      cout << "ERROR: color of starting player provided is invalid.\n";
      exit(1);
    }
  }
  
  //--- RUN GAME
  try {
    game = Checkers(p1, p2, b, starting_turn);
    cout << game << endl;

    for( int i=0; i < MAX_TURNS && game.do_turn(); i++ ) {
      cout << "Turn #" << i << "\n" << game << endl;
    }
  } catch( GameEx e ) {
    cout << "\nERROR: " << e << endl;
    cout << game << endl;
  }

  return 0;
}
///////////////////////////////////////////////////////////////////////////////
