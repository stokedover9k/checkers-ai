#ifndef __DEF_CHECKERS_EVAL__
#define __DEF_CHECKERS_EVAL__

#include <list>
#include <set>
#include <iostream>

#include "checkers-env.h"
#include "exceptions.h"

#define INFINITY 1000000
#define MINIMAX_MAX_DEPTH 8

using namespace std;

namespace EvalState {
  /* count player's pieces (+1), subtract opponent's pieces (-1) */
  float count_pieces(const Board& b, int is_color);

  /* count player's kings (+1), subtract opponent's kings (-1) */
  float count_kings(const Board& b, int is_color);

  /* for each of player's pieces count +1 for each of the two neighbores
     behind it that are not empty. */
  float defense(const Board& b, int is_color);

  /* for each of player's pieces, count +1 for each direction (a1->c3 = c3->a1,
     does NOT get counted twice) in which the piece cannot be taken. The count
     is then multiplied by the number of opponent's kings + 1. */
  float defense_kings(const Board& b, int is_color);

  /* count +1 for each of the player's pieces which are near a wall, -1 for each
     of the opponent's pieces next to a wall. NOTE: "wall" = side wall. */
  float defense_sides(const Board& b, int is_color);

  /* counts +1 for each direction each piece can move in (including jumps). 
     Returns the ratio of the player's counts to the opponent's counts. If, no 
     player moves are available, returns -INFINITY. If no enemy moves - returns 
     INFINITY. */
  float dynamic_position(const Board& b, int is_color);

  /* Sums the distances of each of the pieces from the back of the board (for
     red, back of the board is top, for white - bottom; the back row is counted
     as 1). Returns the player's sum minus the opponent's sum. Returns
     -INFINITY+1 or INFINITY-1 if player has no pieces or opponent has no
     pieces, respectively. */
  float forward_position(const Board& b, int is_color);
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
	  float (*eval_state)(const Board&, int) = EvalState::count_pieces );

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
