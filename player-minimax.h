#ifndef __DEF_MINIMAX_PLAYER
#define __DEF_MINIMAX_PLAYER

#include "player.h"
#include "checkers-env.h"
#include "checkers-eval.h"

using namespace std;

class Minimax_Player : public Player {
 public: 
  Minimax_Player(const string& name, const int c);
  Minimax_Player(const string& name, const Board& b, const int c);
  Minimax_Player(const string& name, float (*eval_state)(const Board& s, int c),
		 int search_depth, int colour);

  list<Loc> get_moves();

 private:
  int _search_depth;
  float (*_eval_func_ptr)(const Board& s, int c);
};

#endif
