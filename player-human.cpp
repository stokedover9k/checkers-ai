#include "player-human.h"

Human_Player::Human_Player(const string& n, const int colour) :
  Player(n, colour)
{  }
Human_Player::Human_Player(const string& n, const Board& b, const int colour) :
  Player(n, b, colour)
{  }

list<Loc> Human_Player::get_moves() {
  list<Loc> moves;
  Loc from = get_move_from();
  moves.push_back(from);
  Move m;
  m.to = from;
  int move_eval = INVALID;

  do {
    m.from = m.to;
    m.to = get_move_to(m.from);
    moves.push_back(m.to);
    
    move_eval = board.eval_move(m, color);
    if( move_eval == INVALID ) 
      throw GameEx("Player::get_moves", 
		   "Invalid move whose destination is given", m.to.x, m.to.y);

    board.move_piece(m);
    if( move_eval & JUMP )
      board.setloc( Loc((m.from.x+m.to.x)/2, (m.from.y+m.to.y)/2), EMPTY );
  } while (!(move_eval & KINGED) && move_eval & JUMP &&
	   board.can_jump_from(m.to));

  return moves;
}

Loc Human_Player::get_move_from() {
  int x, y;
  cout << name << " (color: " << color 
       << ")\nEnter which piece to move (space separated x and y): ";
  cin >> x >> y;
  return Loc(x-1, y-1);
}

Loc Human_Player::get_move_to(const Loc& from) {
  int x, y;
  cout << "Enter where to move to: ";  cin >> x >> y;
  return Loc(x-1, y-1);
}
