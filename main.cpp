/*
  options:

  -s filename
  -state filename
  starts game from the state described in file called filename

  -r filename
  -record filename
  logs the game states into the file called filename

  -r2 filename
  -record2 filename
  logs the game states into the file called filename

  -c color
  -color_start color
  where color = r|red|w|white
  Start the game with this color going first
    
  -p color type
  -player color type
  color = r|red|w|white
  type = first|random|human|minimax
  If type is minimax, immediately following it must be 
    -d depth w1, w2, ...
  where depth is the minimax search depth and w's are weights for the linear
  combination of the evaluation functions used to evaluate states.

 */

#include <iostream>
#include <fstream>
#include "checkers-game.h"
#include "player.h"
#include "player-human.h"
#include "player-basic.h"
#include "player-minimax.h"
#include "checkers-eval.h"
#include <string>
#include <stdlib.h>
#include <string.h>

#define MAX_TURNS 150
#define MAX_EVAL_FUNCTIONS 8

using namespace std;

int p1_eval_weights[MAX_EVAL_FUNCTIONS];
int p2_eval_weights[MAX_EVAL_FUNCTIONS];
float p1_eval_state(const Board& b, int is_color);
float p2_eval_state(const Board& b, int is_color);
float comb_eval_state(const Board& b, int is_color, int *weights);

//typedef float (*)(const Board& b, int) EvalFunc;
float (*minimax_eval_functions[])(const Board&, int) = {
  EvalState::count_pieces,
  EvalState::count_kings,
  EvalState::defense,
  EvalState::defense_kings,
  EvalState::defense_sides,
  EvalState::dynamic_position,
  EvalState::forward_position,
  EvalState::win_or_lose
};

//args: board array, filename
void load_state(int*, char*);

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
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
  };

  Player *p1 = new Random_Player("DEFAULT: Red Random", IS_RED);
  Player *p2 = new Human_Player("DEFAULT: White Human", IS_WHITE);

  int starting_turn = 0;

  ofstream *game_record   = NULL;
  ofstream *game_record_2 = NULL;

  try {
    for( char** arg = argv+1; arg < argv+argc; arg++ ) {
      cout << "argument: " << *arg << endl;

      // load board  [-s|-state]
      if( 0 == strcmp(*arg, "-s") || 
	  0 == strcmp(*arg, "-state" ) ) {
	load_state(s, *++arg);
      }
      // record game states [-r|-record]
      else if( 0 == strcmp(*arg, "-r") ||
	       0 == strcmp(*arg, "-record" ) ) {
	game_record = new ofstream(*++arg);
      }
      // record game states [-r2|-record2]
      else if( 0 == strcmp(*arg, "-r2") ||
	       0 == strcmp(*arg, "-record2") ) {
	game_record_2 = new ofstream(*++arg);
      }
      // load player (EX: "-player red random");
      else if( 0 == strcmp( *arg, "-p" ) ||
	       0 == strcmp( *arg, "-player" ) ) {

	++arg;  //color
	int player_color = -1;
	if(      0 == strcmp(*arg, "r") || 0 == strcmp(*arg, "red") ) {
	  player_color = IS_RED;
	}
	else if( 0 == strcmp(*arg, "w") || 0 == strcmp(*arg, "white") ) {
	  player_color = IS_WHITE;
	}
	else
	  throw GameEx("main", "-player color invalid", player_color);

	Player **p = (player_color == IS_RED) ? &p1 : &p2;
	delete *p;
	
	char *player_type = *++arg;
	string player_name;
	if( player_color == IS_RED ) player_name = "Red ";
	else                         player_name = "White ";
	player_name += player_type;
	
	if     ( 0 == strcmp(player_type, "first"  ) ) { *p = new First_Player( player_name, player_color);  cout << "created player: first"  << endl; }
	else if( 0 == strcmp(player_type, "random" ) ) { *p = new Random_Player(player_name, player_color);  cout << "created player: random" << endl; }
	else if( 0 == strcmp(player_type, "human"  ) ) { *p = new Human_Player( player_name, player_color);  cout << "created player: human"  << endl; }
	else if( 0 == strcmp(player_type, "minimax") ) {       //create the minimax player
	  int minimax_depth = 0;
	  if( 0 == strcmp(*++arg, "-d") )  minimax_depth = atoi(*++arg);
	  else 
	    throw GameEx("main", "-minimax player - no depth provided");
	  if( minimax_depth < 1 )
	    throw GameEx("main", "-minimax player - invalid value for search depth", minimax_depth);

	  int *p_w = (player_color == IS_RED) ? p1_eval_weights : p2_eval_weights;
	  for( int i=0; i<MAX_EVAL_FUNCTIONS; i++ )
	    p_w[i] = atoi(*++arg);
	  
	  *p = new Minimax_Player(player_name,
				  (player_color == IS_RED) ? p1_eval_state : p2_eval_state, 
				  minimax_depth, player_color);
	}
	else 
	  throw GameEx("main", "-player type invalid");
      }
      // starting color
      else if( 0 == strcmp( *arg, "-c" ) ||
	       0 == strcmp( *arg, "-color_start" ) ) {
	string start_color(*++arg);
	if     ( start_color == "r" || start_color == "red"   ) starting_turn = 0;
	else if( start_color == "w" || start_color == "white" ) starting_turn = 1;
	else
	  throw GameEx("main", "-starting color option invalid");
      }
    }
  } catch( GameEx e ) {
    cout << "\nERROR during setup: " << e << endl;
    exit(1);
  }
  


  //---SET UP GAME
  Checkers game;
  Board b(s, BOARD_WIDTH * BOARD_HEIGHT);

  try {
    game = Checkers(p1, p2, b, starting_turn);
    cout << game << endl;
  } catch( GameEx e ) {
    cout << "\nERROR in setting up game: " << e << endl;
    cout << game << endl;
  }

  //---RUN GAME
  for( int i=0; i < MAX_TURNS; i++ ) {
    bool turn_success;
    try {
      turn_success = game.do_turn();
    } catch( GameEx e ) {
      char c;
      cout << "\nERROR in game turn: " << e << endl;
      cout << "Do you wish to replay this turn? (y/n): ";  cin >> c;
      
      if( c != 'y' && c != 'Y' ) return 1;

      cout << game << endl;  i--; continue;  //replay turn
    }

    if( !turn_success ) {     //someone won
      cout << game << endl << "turn " << i << endl;
      break;
    }

    cout << "turn " << i << " complete" << endl;
    if( game_record != NULL ) {
      *game_record << "turn " << i << endl << game << endl;
    }
    if( game_record_2 != NULL ) {
      *game_record_2 << "turn " << i << endl << game << endl;
    }
  }

  if( game_record != NULL )	delete game_record;
  if( game_record_2 != NULL )   delete game_record_2;
  return 0;
}

void load_state(int* state_ptr, char* filename) {
  ifstream board_in(filename, ifstream::in);
    
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

///////////////////////////////////////////////////////////////////////////////

float p1_eval_state(const Board& b, int c) {
  comb_eval_state(b, c, p1_eval_weights); }
float p2_eval_state(const Board& b, int c) {
  comb_eval_state(b, c, p2_eval_weights); }

float comb_eval_state(const Board& b, int c, int *weights) {
  if( c != IS_RED && c != IS_WHITE )
    throw GameEx("comb_eval_state", "invalid color value", c);
  float total = 0;
  for( int i=0; i < MAX_EVAL_FUNCTIONS; i++ ) {
    if( weights[i] == 0 )  continue;
    total += static_cast<float>(weights[i]) * minimax_eval_functions[i](b, c);
  }
}
