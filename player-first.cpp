#include "player-first.h"

First_Player::First_Player(const string& n, const int colour ) : 
  Player(n, colour)
{  }
First_Player::First_Player(const string& n, const Board& b, const int colour ) : 
  Player(n, b, colour)
{  }

list<Loc> First_Player::get_moves() {
  list<Loc> moves;
  
  set<Loc> movable;
  bool must_jump = board.get_move_candidates(movable, color);
  moves.push_back( *(movable.begin()) );

  if( must_jump ) {   //jumps
    do {
      int move_eval = INVALID;
      Loc dest = get_jump_dest( moves.back(), &move_eval );
      board.move_piece( Move( moves.back(), dest ) );
      Loc mid((moves.back().x + dest.x)/2, (moves.back().y + dest.y)/2);
      board.setloc(mid, EMPTY);
    
      moves.push_back(dest);
      must_jump = move_eval & KINGED ? false : true;
    } while( must_jump && board.can_jump_from(moves.back()) );
  }
  else {             //moves
    Loc dest = get_step_dest( moves.back() );
    moves.push_back(dest);
  }

  return moves;
}

Loc First_Player::get_step_dest( const Loc& from ) {
  int w = board.get_width();
  int h = board.get_height();
  int e = INVALID;
  if( from.y > 0 ) {
    if( from.x > 0 ) {
      Loc to(from.x-1, from.y-1);
      if( board.eval_move( Move(from, to), color ) != INVALID ) {
	return to;
      }
    }
    if( from.x < w-1 ) {
      Loc to(from.x+1, from.y-1);
      if( board.eval_move( Move(from, to), color ) != INVALID ) {
	return to;
      }
    }
  }
  if( from.y < h-1 ) {
    if( from.x > 0 ) {
      Loc to(from.x-1, from.y+1);
      if( board.eval_move( Move(from, to), color ) != INVALID ) {
	return to;
      }
    }
    if( from.x < w-1 ) {
      Loc to(from.x+1, from.y+1);
      if( board.eval_move( Move(from, to), color ) != INVALID ) {
	return to;
      }
    }
  }
}

Loc First_Player::get_jump_dest( const Loc& from, int* move_eval ) {
  int w = board.get_width();
  int h = board.get_height();
  int e = INVALID;

  if( from.y > 1 ) {
    if( from.x > 1 ) {
      Loc to = Loc(from.x-2, from.y-2);
      if( (e = board.eval_move( Move(from, to), color)) != INVALID ) {
	*move_eval = e;
	return to;
      }
    }
    if( from.x < w-2 ) {
      Loc to = Loc(from.x+2, from.y-2);
      if( (e = board.eval_move( Move(from, to), color)) != INVALID ) {
	*move_eval = e;
	return to;
      }
    }
  }
  if( from.y < h-2 ) {
    if( from.x > 1 ) {
      Loc to = Loc(from.x-2, from.y+2);
      if( (e = board.eval_move( Move(from, to), color)) != INVALID ) {
	*move_eval = e;
	return to;
      }
    }
    if( from.x < w-2 ) {
      Loc to = Loc(from.x+2, from.y+2);
      if( (e = board.eval_move( Move(from, to), color)) != INVALID ) {
	*move_eval = e;
	return to;
      }
    }
  }
}
