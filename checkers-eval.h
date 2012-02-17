#ifndef __DEF_CHECKERS_EVAL__
#define __DEF_CHECKERS_EVAL__

#include <list>
#include <set>
#include <iostream>

#include "checkers-env.h"
#include "exceptions.h"

#define INFINITY 10000000
#define MINIMAX_MAX_DEPTH 8

using namespace std;

namespace EvalState {
  float count_pieces(const Board& b, int is_color);
  float count_kings(const Board& b, int is_color);
};

typedef list<Loc> Action;

/////////////////////////////////////////////////////////////////////////////

class StateVal {
 public:
  StateVal(const Action& action, float value);
  bool operator < (const StateVal& s) const;

  Action action() const;
  float value() const;
    
 private:
  Action _a;
  float _v;
};

/////////////////////////////////////////////////////////////////////////////

class Minimax {
 public:
  Minimax(int c, 
	  float (*eval_state)(const Board&, int) = &EvalState::count_pieces );

  StateVal minimax_decision(const Board& state, int depth);

 private:
  int _color;
  int _max_depth;
  Action _last_max_a;
  Action _last_min_a;
  //pointer to the evaluation function
  float (*_eval_state_func_ptr)(const Board& b, int c);
  
  //--- functions:

  //depth is how many more levels to search
  float max_value(const Board& state, int depth, float alpha, float beta, 
		  int color=EMPTY);
  float min_value(const Board& state, int depth, float alpha, float beta, 
		  int enemy_color=EMPTY);

  set<Action>& possible_actions(const Board& state, int color) const;
  set<Action> get_jump_moves_from(const Loc& from, const Board& b) const;

  bool terminal_state(const Board& state, int color, int current_search_depth);

  //if color is EMPTY, returns the opposite of _color member
  int _other_color(int color=EMPTY);

  float _eval_state(const Board& b, int c = EMPTY);
};
#endif
