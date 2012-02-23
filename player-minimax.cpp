#include "player-minimax.h"

Minimax_Player::Minimax_Player(const string& n, const int c) : Player(n, c)
{  }
Minimax_Player::Minimax_Player(const string& n, const Board& b, const int c) :
  Player( n, b, c )
{  }
Minimax_Player::Minimax_Player(const string& n, 
			       float(*eval_state)(const Board& s, int c), 
			       int search_depth, int colour) :
  Player( n, colour ), _search_depth(search_depth) {
  _eval_func_ptr = eval_state; 
  if( search_depth < 1 ) 
    throw GameEx("Minimax_Player::Minimax_Player",
		 "invalid search depth value", search_depth);
		 
}

list<Loc> Minimax_Player::get_moves() {
  cout << name << " generating move..." << endl;
  Minimax minimax(color, _eval_func_ptr);
  return minimax.minimax_decision(board, _search_depth).action();
}

//float (Minimax_Player::*_eval_state)(const Board& s, int c) const = NULL;
