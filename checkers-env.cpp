#include "checkers-env.h"

Loc::Loc(int xi, int yi) : x(xi), y(yi) {
  if( x >= BOARD_WIDTH )
    throw GameEx("Loc::Loc", "x >= WIDTH", x, BOARD_WIDTH);
  if( y >= BOARD_HEIGHT )
    throw GameEx("Loc::Loc", "y >= HEIGHT", y, BOARD_HEIGHT);
}

bool Loc::operator == (const Loc& l) const {
  return x == l.x && y == l.y;
}

bool Loc::operator < (const Loc& l) const {
  return y < l.y || (y == l.y && x < l.x);
}

ostream& operator << (ostream& s, const Loc& l) {
  s << "(" << l.x+1 << "," << l.y+1 << ")";
  return s;
}

///////////////////////////////////////////////////////////////////////////////
Move::Move() : from(0,0), to(0,0)
{  }

Move::Move(const Loc& f, const Loc& t) : from(f), to(t)
{  }

ostream& operator << (ostream& s, const Move& m) {
  s << "Move " << m.from << "->" << m.to;
  return s;
}
  
///////////////////////////////////////////////////////////////////////////////
set<Loc> *Board::_valid_locs = 0;

Board::Board() {
  empty();
}

Board::Board(const Board& b) {
  for( int x=BOARD_WIDTH*BOARD_HEIGHT/2-1; x>=0; x-- )
      board[x] = b.board[x];
}

Board::Board(const int* brd, int len) {
  if( len != BOARD_WIDTH * BOARD_HEIGHT )
    throw GameEx("Board::Board(int**, int, int)", 
		 "Invalid board dimensions", len, BOARD_WIDTH * BOARD_HEIGHT);

  for( int y=0; y<BOARD_HEIGHT; y++ ) {
    for( int x=0; x<BOARD_WIDTH; x++ ) {
      setloc( Loc(x,y), *(brd++) );
    }
  }
}

Board& Board::operator = (const Board& b) {
  if( this == &b ) return *this;
  for( int x=BOARD_WIDTH*BOARD_HEIGHT/2; x>=0; x-- )
      board[x] = b.board[x];
  return *this;
}

void Board::empty() {
  for( int x=BOARD_WIDTH*BOARD_HEIGHT/2-1; x>=0; x-- )
      board[x] = EMPTY;
}

int Board::get_width()  const { return BOARD_WIDTH;  }
int Board::get_height() const { return BOARD_HEIGHT; }

int Board::get(const Loc& l) const {
  if( (l.x + l.y) % 2 == 0 ) return EMPTY;
  return board[l.y*4 + l.x/2];
}

set<Loc>& Board::get_neighbours(const Loc& l) {
  set<Loc>& s = *new set<Loc>();
  if( l.y > 0 ) {
    if( l.x > 0 )              s.insert(Loc(l.x-1, l.y-1));
    if( l.x < BOARD_WIDTH-1 )  s.insert(Loc(l.x+1, l.y-1));
  }
  if( l.y < BOARD_HEIGHT-1 ) {
    if( l.x > 0 )              s.insert(Loc(l.x-1, l.y+1));
    if( l.x < BOARD_WIDTH-1 )  s.insert(Loc(l.x+1, l.y+1));
  }
  return s;
}

set<Loc>& Board::get_jump_neighbours(const Loc& l) {
  set<Loc>& s = *new set<Loc>();
  if( l.y > 1 ) {
    if( l.x > 1 )              s.insert(Loc(l.x-2, l.y-2));
    if( l.x < BOARD_WIDTH-2 )  s.insert(Loc(l.x+2, l.y-2));
  }
  if( l.y < BOARD_HEIGHT-2 ) {
    if( l.x > 1 )              s.insert(Loc(l.x-2, l.y+2));
    if( l.x < BOARD_WIDTH-2 )  s.insert(Loc(l.x+2, l.y+2));
  }
  return s;
}

void Board::setloc(int x, int y, int val) {
  try { setloc(Loc(x,y), val); }
  catch(GameEx e) { e.add_trace("Board::setloc(int, int, int)");  throw e; }
}

void Board::setloc(const Loc& l, int val) {
  if( (l.x + l.y) % 2 == 0 ) return;
  if( !square_valid(val) ) 
    throw GameEx("Board::set", "illegal square value", val);
  board[l.y*4 + l.x/2] = val;
}

void Board::move_piece(const Move& m) {
  setloc(m.to, get(m.from));
  setloc(m.from, EMPTY);
  if( get(m.to) == RED && m.to.y == BOARD_HEIGHT-1 )
    setloc(m.to, RED_KING);
  else if( get(m.to) == WHITE && m.to.y == 0 )
    setloc(m.to, WHITE_KING);
}

void Board::jump_piece(const Move& m) {
  move_piece(m);
  setloc( Loc((m.from.x+m.to.x)/2, (m.from.y+m.to.y)/2), EMPTY );
}

bool Board::square_valid(int v) {
  if( v == EMPTY      ) return true;
  if( v == RED        ) return true;
  if( v == WHITE      ) return true;
  if( v == RED_KING   ) return true;
  if( v == WHITE_KING ) return true;
  return false;
}

const set<Loc>& Board::valid_locs(void) {
  if( _valid_locs == 0 ) {
    _valid_locs = new set<Loc>();
    for( int y=0; y<BOARD_HEIGHT; y++ )
      for( int x=0; x<BOARD_WIDTH; x++ )
	if( (x+y) % 2 == 1 )
	  _valid_locs->insert(Loc(x,y));
  }
  return *_valid_locs;
}

bool Board::can_jump_from(const Loc& l) const {
  int height = get_height();
  int width  = get_width();
  int p = get(l);
  if( p == EMPTY ) return false;
  int enemy_color = (p & IS_RED) ? IS_WHITE : IS_RED;

  if( l.y > 1 && (p & IS_WHITE || p==RED_KING) ) {         //check upward
    if( l.x > 1 ) {                                        //--to the left
      if( get(Loc(l.x-1, l.y-1)) & enemy_color &&
	  get(Loc(l.x-2, l.y-2)) == EMPTY ) return true;
    }
    if( l.x < width-2 ) {                                  //--to the right
      if( get(Loc(l.x+1, l.y-1)) & enemy_color &&
	  get(Loc(l.x+2, l.y-2)) == EMPTY ) return true;
    }
  }
  if( l.y < height-2 && (p & IS_RED || p==WHITE_KING) ) {  //check downward
    if( l.x > 1 ) {                                        //--to the left
      if( get(Loc(l.x-1, l.y+1)) & enemy_color &&
	  get(Loc(l.x-2, l.y+2)) == EMPTY ) return true;
    }
    if( l.x < width-2 ) {                                  //--to the right
      if( get(Loc(l.x+1, l.y+1)) & enemy_color &&
	  get(Loc(l.x+2, l.y+2)) == EMPTY ) return true;
    }
  }
  return false;
}

bool Board::can_move_from(const Loc& l) const {
  int p = get(l);
  if( p == EMPTY ) return false;
  
  if( l.y > 0 && (p & IS_WHITE || p == RED_KING) ) {            //check upward
    if( l.x > 0 )                                               //--to the left
      if( get(Loc(l.x-1, l.y-1)) == EMPTY ) return true;
    if( l.x < BOARD_WIDTH-1 )                                   //--to the right
      if( get(Loc(l.x+1, l.y-1)) == EMPTY ) return true;
  }
  if( l.y < BOARD_HEIGHT-1 && (p & IS_RED || p == WHITE_KING) ) {  //check down
    if( l.x > 0 )
      if( get(Loc(l.x-1, l.y+1)) == EMPTY ) return true;
    if( l.x < BOARD_WIDTH-1 )
      if( get(Loc(l.x+1, l.y+1)) == EMPTY ) return true;
  }
  return false;
}

bool Board::get_move_candidates(set<Loc, less<Loc> >& dest, int color) const {
  if( color != IS_RED && color != IS_WHITE )
    throw GameEx("Board::get_move_candidates", "invalid color value", color);
  
  dest.clear();
  bool no_jumps = true;
  for( int x=0; x<BOARD_WIDTH; x++ ) {
    for( int y=0; y<BOARD_HEIGHT; y++ ) {
      if( get(Loc(x,y)) & color ) {
	Loc l(x, y);
	if( can_jump_from(l) ) {
	  if( no_jumps ) {
	    dest.clear();
	    no_jumps = false;
	  }
	  dest.insert(l);
	}

	if( no_jumps && can_move_from(l) )
	  dest.insert(l);
      }
    } 
  }

  return !no_jumps;
}

int Board::eval_move(const Move& m, int is_color) const {
  int dir = (is_color == IS_RED) ? 1 : -1;
  int sq = get(m.from);

  if( m.from == m.to   ) return INVALID;
  if( !(sq & is_color) ) return INVALID;

  int val = 0;
  int dx = m.to.x - m.from.x;
  int dy = m.to.y - m.from.y;
  
  if( abs(dx) != abs(dy) ) return INVALID;

  //horizontal checks
  if( dx * dir ==  0 ) return INVALID;
  if( dx * dir >   2 ) return INVALID;
  if( dx * dir < 0 ) val |= RIGHT;
  if( dx * dir > 0 ) val |= LEFT;

  //vertical checks
  if( dy * dir >  0 ) val |= FORWARD;
  if( dy * dir <  0 ) val |= BACKWARD;
  if( abs(dy)  == 2 ) val |= JUMP;
  
  //other checks
  if( get(m.to) != EMPTY )             return INVALID; //check destination
  if( (val & BACKWARD) && (sq & IS_KING) == 0 )  return INVALID; //must be king
  if( val & JUMP ) {
    int mid_sq = get( Loc((m.from.x + m.to.x)/2, (m.from.y + m.to.y)/2) );
    if( mid_sq == EMPTY || mid_sq & is_color )    //empty or our color
      return INVALID;
  }
  if( (get(m.from) == RED && m.to.y == BOARD_HEIGHT-1) ||
      (get(m.from) == WHITE && m.to.y == 0) )
    val |= KINGED;
  
  return val;
}

ostream& operator << (ostream& s, const Board& b) {
  s << "  | 1   2   3   4   5   6   7   8  \n"
    << "__|________________________________";
  for( int y=0; y<BOARD_HEIGHT; y++ ) {
    s << endl << y+1 << " |";
    for( int x=0; x<BOARD_WIDTH; x++ ) {
      if( (x+y) % 2 == 0 ) {
	s << "||||";
      } else {
	switch( b.get(Loc(x,y)) ) 
	  {
	  case RED       : s << " r |";  break;
	  case WHITE     : s << " w |";  break;
	  case RED_KING  : s << "[R]|";  break;
	  case WHITE_KING: s << "[W]|";  break;
	  default        : s << "   |";
	  }
      }
    }
    if( (y) % 2 == 0 )  s << "\n  |||||___|||||___|||||___|||||___|";
    else                s << "\n  |___|||||___|||||___|||||___|||||";
  }
      
  return s;
}
