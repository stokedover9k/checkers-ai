#ifndef __DEF_FIRST_PLAYER__
#define __DEF_FIRST_PLAYER__

#include <cstdlib>
#include <time.h>
#include "player.h"
#include "checkers-env.h"

using namespace std;

class First_Player : public Player {
 public:
  First_Player(const string& player_name, const int color);
  First_Player(const string& player_name, const Board& b, const int colour);

  list<Loc> get_moves();
  
 private:
  Loc get_jump_dest( const Loc& from, int* move_eval );
  Loc get_step_dest( const Loc& from );
};

///////////////////////////////////////////////////////////////////////////////

class Random_Player : public Player {
 public:
  Random_Player(const string& player_name, const int color, 
		bool rand_generator_seeded=false);

  list<Loc> get_moves();

 private:
  static bool _rand_generator_seeded;
};

#endif
