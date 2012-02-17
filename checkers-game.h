#ifndef __DEF_CHECKERS_GAME_
#define __DEF_CHECKERS_GAME_

#include "checkers-env.h"
#include "exceptions.h"
#include "player.h"
#include <iostream>
#include <list>

using namespace std;

class Checkers {
 public:
  Checkers();
  Checkers(Player *player_1, Player *player_2);  //RED, WHITE
  Checkers(Player *player_1, Player *player_2, const Board& b, int turn=0);

  /* returns true if the turn can be taken and is taken
     returns false if no turns are available
     throws GameEx exception if something goes wrong */
  bool do_turn();
  
  friend ostream& operator << (ostream&, Checkers&);

 private:
  Board board;
  Player *p1;    //RED
  Player *p2;    //WHITE
  int turn_num;
};

#endif
