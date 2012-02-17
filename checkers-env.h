#ifndef __DEF_CHECKERS_ENV_
#define __DEF_CHECKERS_ENV_

#include <iostream>
#include <cstdlib>
#include <set>
#include "exceptions.h"

#define BOARD_HEIGHT 8
#define BOARD_WIDTH 8

using namespace std;

enum SQ {
  NOT_EMPTY = 1,
  IS_RED    = 1 << 1,
  IS_WHITE  = 1 << 2,
  IS_KING   = 1 << 3
};

enum {
  START      = 1,
  EMPTY      = 0,
  RED        = NOT_EMPTY | IS_RED,
  WHITE      = NOT_EMPTY | IS_WHITE,
  RED_KING   = NOT_EMPTY | IS_KING | IS_RED,
  WHITE_KING = NOT_EMPTY | IS_KING | IS_WHITE
};

enum {
  INVALID  = 0,
  FORWARD  = 1,
  BACKWARD = 1 << 1,
  LEFT     = 1 << 2,
  RIGHT    = 1 << 3,
  JUMP     = 1 << 4,
  KINGED   = 1 << 5
};


class Loc {
 public:
  Loc(int x, int y);

  int x, y;

  bool operator == (const Loc&) const;
  bool operator < (const Loc&) const;
  friend ostream& operator << (ostream&, const Loc&);
  
 private:
  Loc();
};
///////////////////////////////////////////////////////////////////////////////
class Move {
 public:
  Move();
  Move(const Loc& mFrom, const Loc& mTo);
  
  friend ostream& operator << (ostream&, const Move&);

  Loc from, to;

 private:
  friend class Board;
};
///////////////////////////////////////////////////////////////////////////////
class Board {
 public:
  Board();                 //creates an empty board
  Board(const Board&);     //copy constructor
  Board(const int* brd, int array_length);  //length = board height * width

  int get_width()  const;
  int get_height() const;
  int get(const Loc&) const;

  static set<Loc>& get_neighbours(const Loc& l);
  static set<Loc>& get_jump_neighbours(const Loc& l);

  void setloc(const Loc&, int val);

  void move_piece(const Move&);
  void jump_piece(const Move&);

  /* returns the bitmast describing the proposed move
     NOTE: does not execute the move */
  int eval_move(const Move&, int color) const;

  static bool square_valid(int val);

  /* looks at the board, so will return false if if loc is empty */
  bool can_jump_from(const Loc&) const;

  /* looks at the board, and returns true if a move (not necessarily jump) is 
     available */
  bool can_move_from(const Loc&) const;

  /* returns true if candidates are candidates for jumps */
  bool get_move_candidates(set<Loc>& dest, int color) const;

  friend ostream& operator << (ostream&, const Board&);

  Board& operator = (const Board& b);
  
 private:
  void empty();
  void setloc(int x, int y, int val);

  int board[BOARD_WIDTH * BOARD_HEIGHT / 2];
};

#endif
