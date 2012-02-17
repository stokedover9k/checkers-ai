#include "checkers-game.h"

Checkers::Checkers() : board(), turn_num(0)
{  }

Checkers::Checkers(Player *a, Player *b) :
  board(), p1(a), p2(b), turn_num(0)
{  }
Checkers::Checkers(Player *a, Player *b, const Board& brd, int turn) :
  board(brd), p1(a), p2(b), turn_num(turn)
{  }

bool Checkers::do_turn() {
  int color = (turn_num % 2 == 0) ? IS_RED : IS_WHITE;
  Player* p = (color == IS_RED) ? p1 : p2;
  set<Loc, less<Loc> > permitted_moves;
  bool must_jump = board.get_move_candidates(permitted_moves, color);
  if( permitted_moves.size() == 0 ) return false;

  cout << "Starting turn: " << p->get_name() << endl;

  p->set_board(board);
  list<Loc> moves( p->get_moves() );
  cout << "Moves acquired (" << p->get_name() << "). Length: " << moves.size() << endl;

  /*
  for( list<Loc>::iterator i = moves.begin(); i != moves.end(); i++ )
    cout << (*i) << endl;
  */

  Move m;
  m.to = moves.front();
  moves.pop_front();

  int move_eval = INVALID;

  if( permitted_moves.find(m.to) == permitted_moves.end() )
    throw GameEx("Checkers::do_turn", "not allowed to take this move", 
		 m.to.x, m.to.y);

  while( moves.size() > 0 ) {
    m.from = m.to;
    m.to = moves.front();
    moves.pop_front();

    move_eval = board.eval_move(m, color);
    if( move_eval == INVALID )
      throw GameEx("Checkers::do_turn", "Invalid move", m.to.x, m.to.y);
    if( must_jump && move_eval & JUMP == 0 )
      throw GameEx("Checkers::do_turn", "move must be a jump", m.to.x, m.to.y);

    board.move_piece(m);
    if( move_eval & JUMP ) {
      board.setloc( Loc((m.from.x+m.to.x)/2, (m.from.y+m.to.y)/2), EMPTY );
    }

    if( move_eval & KINGED ) {
      board.setloc(m.to, board.get(m.to) | IS_KING);
      break;
    }
  }
  if( moves.size() > 0 )  
    throw GameEx("Checkers::do_turn", 
		 "requested too many moves", m.to.x, m.to.y);

  if( !(move_eval & KINGED) && move_eval & JUMP && board.can_jump_from(m.to) )
    throw GameEx("Checkers::do_turn",
		 "turn terminated too early", m.to.x, m.to.y);

  ++turn_num;
  return true;
}

ostream& operator << (ostream& s, Checkers& c) {
  s << "   RED: " << c.p1->get_name() 
    << "   WHITE: " << c.p2->get_name() << endl
    << c.board;
  return s;
}
