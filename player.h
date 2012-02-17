#ifndef __DEF_PLAYER_
#define __DEF_PLAYER_

#include <string>
#include <list>
#include "checkers-env.h"

using namespace std;

class Player {
 public:
  Player(const string& player_name, const int color);
  Player(const string& player_name, const Board& b, const int colour);

  void set_board(const Board&);
  const string& get_name();

  virtual list<Loc> get_moves() = 0;

 protected:
  string name;
  int color;
  Move last_move;
  Board board;
  int board_copy[BOARD_WIDTH][BOARD_HEIGHT];
};

#endif
