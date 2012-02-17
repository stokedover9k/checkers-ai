#ifndef __DEF_HUMAN_PLAYER__
#define __DEF_HUMAN_PLAYER__

#include "player.h"
#include "checkers-env.h"

using namespace std;

class Human_Player : public Player {
 public: 
  Human_Player(const string& player_name, const int color);
  Human_Player(const string& player_name, const Board& b, const int colour);

  list<Loc> get_moves();

 private:
  Loc get_move_from();
  Loc get_move_to(const Loc& from);

};

#endif
