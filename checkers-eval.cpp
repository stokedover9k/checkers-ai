#include "checkers-eval.h"

float EvalState::count_pieces(const Board& b, int is_color) {
  int enemy_color = (is_color == IS_RED) ? IS_WHITE : IS_RED;
  int total = 0;
  int w = b.get_width();
  int h = b.get_height();

  for( int y=0; y<h; y++ ) {
    for( int x=0; x<w; x++ ) {
      int val = b.get(Loc(x,y));
      if( val == EMPTY )  continue;
      if     ( val & is_color    ) total += 1;
      else if( val & enemy_color ) total -= 1;
    }
  }
  
  return static_cast<float>(total);
}

float EvalState::count_kings(const Board& b, int is_color) {
  int total = 0;
  int w = b.get_width();
  int h = b.get_height();

  for( int y=0; y<h; y++ ) {
    for( int x=0; x<w; x++ ) {
      int val = b.get(Loc(x,y));
      if( val == EMPTY )  continue;
      total += (val & (is_color|IS_KING) ) ? 1 : -1;
    }
  }
  
  return static_cast<float>(total);  
}

float EvalState::defense(const Board& b, int is_color) {
  const set<Loc>& locs = Board::valid_locs();
  int dy = (is_color == IS_RED) ? -1 : 1;
  int ysafe = (is_color == IS_RED) ? 0 : BOARD_HEIGHT-1;
  int total = 0;

  for( set<Loc>::iterator i = locs.begin(); i != locs.end(); i++ ) {
    if( b.get(*i) & is_color ) {
      const Loc l(*i);
      if( l.y == ysafe ) {  total++;  continue;  }  //if all the way back

      if( l.x > 0 ) {              //check to the left
	if( b.get( Loc(l.x-1, l.y+dy) ) & is_color ) total++;
      } 
      else total++;

      if( l.x < BOARD_WIDTH-1 ) {  //check to the right
	if( b.get( Loc(l.x+1, l.y+dy) ) & is_color ) total++;
      }
      else total++;
    }
  }

  return static_cast<float>(total);
}

float EvalState::defense_kings(const Board& b, int col) {
  const set<Loc>& locs = Board::valid_locs();
  int total = 0;
  int enemy_kings = 0;
  int enemy_king_val = (col == IS_RED) ? WHITE_KING : RED_KING;

  for( set<Loc>::iterator i = locs.begin(); i != locs.end(); i++ ) {
    int loc_val = b.get(*i);
    if( loc_val & col ) {
      const Loc l(*i);
      if( l.y == 0 || l.y == BOARD_HEIGHT-1 )
	{  total += 2;  continue;  }
      
      if( l.x == 0 || l.x == BOARD_WIDTH-1 )
	{  total += 2;  continue;  }

      if( b.get( Loc(l.x+1, l.y+1) ) & col || b.get( Loc(l.x-1, l.y-1) ) & col )
	total++;
      if( b.get( Loc(l.x-1, l.y+1) ) & col || b.get( Loc(l.x+1, l.y-1) ) & col )
	total++;
    }
    else if( loc_val & enemy_king_val ) {
      enemy_kings++;
    }
  }

  return static_cast<float>(total * (1 + enemy_kings));
}

float EvalState::defense_sides(const Board& b, int col) {
  int total = 0;
  int enemy = (col == IS_RED) ? IS_WHITE : IS_RED;
  int locval;
  bool even_width = (BOARD_HEIGHT % 2 == 0) ? true : false;
  
  for( int y=0; y<BOARD_HEIGHT; y++ ) {
    if( y % 2 == 1) {
      locval = b.get( Loc(0,y) );
      if( locval & col )        total++;
      else if( locval & enemy)  total--;
      
      if( !even_width ) {
	locval = b.get( Loc(BOARD_WIDTH-1,y) );
	if( locval & col )         total++;
	else if( locval & enemy )  total--;
      }
    }
    else if( even_width ) {
      locval = b.get( Loc(BOARD_WIDTH-1,y) );
      if( locval & col )         total++;
      else if( locval & enemy )  total--;
    }
  }

  return static_cast<float>(total);
}

float EvalState::dynamic_position(const Board& b, int col) {
  const set<Loc>& locs = Board::valid_locs();
  int count_player = 0;
  int count_enemy  = 0;
  int *counter = 0;

  for( set<Loc>::iterator i = locs.begin(); i != locs.end(); i++ ) {
    int locval = b.get(*i);
    if( locval == EMPTY )  continue;

    counter = (locval & col) ? &count_player : &count_enemy;

    set<Loc>& neigh = Board::get_neighbours(*i);
    set<Loc>::iterator neigh_itr;
    Move m(*i, Loc(0,0));

    for( neigh_itr = neigh.begin(); neigh_itr != neigh.end(); neigh_itr++ ) {
      m.to = *neigh_itr;
      if( b.eval_move(m) != INVALID )  (*counter)++;
    }
    
    delete &neigh;
    neigh = Board::get_jump_neighbours(*i);
    
    for( neigh_itr = neigh.begin(); neigh_itr != neigh.end(); neigh_itr++ ) {
      m.to = *neigh_itr;
      if( b.eval_move(m) != INVALID ) (*counter)++;
    }
    delete &neigh;
  }
  
  if( count_player == 0 ) {
    return static_cast<float>(-INFINITY);
  }
  if( count_enemy == 0  ) {
    return static_cast<float>(INFINITY);
  }
  return static_cast<float>(count_player) / static_cast<float>(count_enemy);
}  

float EvalState::forward_position(const Board& b, int col) {
  const set<Loc>& locs = Board::valid_locs();
  int count_player = 0;
  int count_enemy = 0;
  int *counter = 0;

  for( set<Loc>::iterator i = locs.begin(); i != locs.end(); i++ ) {
    int locval = b.get(*i);
    if( locval == EMPTY )  continue;

    counter = (locval & col) ? &count_player : &count_enemy;

    (*counter) += ((locval & IS_RED) ? (*i).y + 1 : BOARD_HEIGHT - (*i).y);
  }

  if( count_player == 0 ) return static_cast<float>(-INFINITY);
  if( count_enemy  == 0 ) return static_cast<float>( INFINITY);
  return static_cast<float>(count_player - count_enemy);
}

float EvalState::win_or_lose(const Board& b, int is_color) {
  set<Loc> locs;
  int enemy = (is_color == IS_RED) ? IS_WHITE : IS_RED;

  b.get_move_candidates(locs, enemy);
  if( locs.size() == 0 ) {
    return static_cast<float>(INFINITY);
  }

  b.get_move_candidates(locs, is_color);
  if( locs.size() == 0 ) {
    return static_cast<float>(-INFINITY);
  }
  return static_cast<float>(0);
}

///////////////////////////////////////////////////////////////////////////////

StateVal::StateVal(const Action& a, float v) : _a(a), _v(v)
{  }

bool StateVal::operator < (const StateVal& s) const { 
  return _v < s._v;
}

Action StateVal::action() const { return _a; }
float StateVal::value() const { return _v; }

///////////////////////////////////////////////////////////////////////////////

Minimax::Minimax(int c, float (*eval_state)(const Board&, int)) : _color(c) {
  if( c != IS_RED && c != IS_WHITE )
    throw GameEx("Minimax::Minimax(int)", "invalid color", c);
  _eval_state_func_ptr = eval_state;
}

StateVal Minimax::minimax_decision(const Board& state, int depth) {
  if( depth > MINIMAX_MAX_DEPTH )
    throw GameEx("Minimax::minimax_decision", "depth too high (>MAX_DEPTH)",
		 depth, MINIMAX_MAX_DEPTH);
  
  float max_val = max_value(state, depth, -LARGE_INFINITY, 
			    LARGE_INFINITY, _color);
  return StateVal(_last_max_a, max_val);
}

float Minimax::max_value(const Board& state, int depth, 
			 float alpha, float beta, int color) {
  if(color == EMPTY) color = _color;

  if( terminal_state(state, color, depth) ) {
    float v = _eval_state(state, color);
    //cout << "-max: state terminal: " << v << endl;
    return v;
  }
  
  float max_v = -LARGE_INFINITY;
  float tmp_v;
  Board tmp_b;
  set<Action>::iterator max_a;
  
  set<Action>& actions = Player::possible_actions(state, color);
  set<Action>::iterator itr;

  /*
  cout << "--possible actions(max): " << actions.size() << endl;
  for( set<Action>::const_iterator out_i = actions.begin();
       out_i != actions.end(); out_i++ ) {
    cout << "---a: " << *(*out_i).begin() << "-->";
    cout << *--(*out_i).end() << endl;
  }
  //*/
  for( itr = actions.begin(); itr != actions.end(); itr++ ) { //itr thru actions
    tmp_b = Board(state);
    Action::const_iterator loc_itr = (*itr).begin();
    Move m( *loc_itr++, Loc(0,0) );
    m.to = *loc_itr;
    int move_eval = tmp_b.eval_move(m, color);
    if( move_eval & JUMP ) tmp_b.jump_piece(m);
    else                   tmp_b.move_piece(m);
    
    while( (++loc_itr) != (*itr).end() ) {      //create temp state
      m.from = m.to;
      m.to = *loc_itr;
      tmp_b.jump_piece(m);
      /*
      else {
	tmp_b.move_piece(m);
      }
      */
    }

    tmp_v = min_value(tmp_b, depth, alpha, beta, _other_color(color));
    if( tmp_v >= max_v ) {
      max_v = tmp_v;
      max_a = itr;
    }

    if( max_v >= beta ) {
      _last_max_a = Action(*itr);
      delete &actions;
      return max_v;
    }
    
    alpha = (alpha > max_v) ? alpha : max_v;
  }

  _last_max_a = Action(*max_a);
  delete &actions;
  return max_v;
}

float Minimax::min_value(const Board& state, int depth, 
			 float alpha, float beta, int color) {
  if(color == EMPTY) color = _other_color(_color);

  if( terminal_state(state, color, depth) ) {
    float v = _eval_state(state, _color);
    //cout << "-min: state terminal: " << v << endl;
    return v;
  }

  float min_v = LARGE_INFINITY;
  float tmp_v;
  Board tmp_b;
  set<Action>::iterator min_a;
  
  set<Action>& actions = Player::possible_actions(state, color);
  set<Action>::iterator itr;

  /*  
  cout << "--possible actions(min): " << actions.size() << endl;
  cout << state << endl;
  for( set<Action>::const_iterator out_i = actions.begin();
       out_i != actions.end(); out_i++ ) {
    cout << "---a: " << *(*out_i).begin() << "-->";
    cout << *--(*out_i).end() << endl;
  }
  //*/
  for( itr = actions.begin(); itr != actions.end(); itr++ ) { //itr thru actions
    tmp_b = Board(state);
    Action::const_iterator loc_itr = (*itr).begin();
    Move m( *loc_itr++, Loc(0,0) );
    m.to = *loc_itr;
    int move_eval = tmp_b.eval_move(m, color);
    if( move_eval & JUMP ) tmp_b.jump_piece(m);
    else                   tmp_b.move_piece(m);

    while( (++loc_itr) != (*itr).end() ) {      //create temp state
      m.from = m.to;
      m.to = *loc_itr;
      tmp_b.jump_piece(m);
      /*
      else {
	tmp_b.move_piece(m);
      }
      */
    }

    tmp_v = max_value(tmp_b, depth-1, alpha, beta, _other_color(color));
    if( tmp_v <= min_v ) {
      min_v = tmp_v;
      min_a = itr;
    }

    if( min_v <= alpha ) {
      _last_min_a = Action(*itr);
      delete &actions;
      return min_v;
    }

    beta = (beta < min_v) ? beta : min_v;
  }

  _last_min_a = Action(*min_a);
  delete &actions;
  return min_v;
}

bool Minimax::terminal_state(const Board& state, int color, int depth) {
  set<Loc> candidates;
  if( state.get_move_candidates(candidates, color) ) return false;
  if( depth <= 0 || candidates.size() == 0 ) return true;
  return false;
}

int Minimax::_other_color(int color) { 
  if( color == EMPTY ) int color = _color;
  return (color == IS_RED) ? IS_WHITE : IS_RED;
}

float Minimax::_eval_state(const Board& b, int c) {
  if( c == EMPTY ) c = _color;
  float v = _eval_state_func_ptr(b, c);
  return v;
}
