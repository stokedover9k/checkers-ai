#ifndef __DEF_PLAYER_
#define __DEF_PLAYER_

#include <string>
#include <list>
#include "checkers-env.h"

using namespace std;

typedef list<Loc> Action;

class Player {
 public:
  Player(const string& player_name, const int color);
  Player(const string& player_name, const Board& b, const int colour);

  void set_board(const Board&);
  const string& get_name();
  const int get_color(void);

  virtual list<Loc> get_moves() = 0;

  static set<Action>& possible_actions(const Board& s, int color);
  static set<Action>& get_jump_moves_from(const Loc& from, const Board& b);

 protected:
  string name;
  int color;
  Move last_move;
  Board board;
  int board_copy[BOARD_WIDTH][BOARD_HEIGHT];

};

#endif
