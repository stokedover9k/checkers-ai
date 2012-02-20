#include "checkers-eval.h"

float EvalState::count_pieces(const Board& b, int is_color) {
  if( is_color != IS_RED && is_color != IS_WHITE )
    throw GameEx("EvalState::count_pieces", "invalid color", is_color);

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
  if( is_color != IS_RED && is_color != IS_WHITE )
    throw GameEx("EvalState::piece_kings", "invalid color", is_color);

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
  if( is_color != IS_RED && is_color != IS_WHITE )
    throw GameEx("EvalState::defense", "invalid color", is_color);
  
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
  if( col != IS_RED && col != IS_WHITE )
    throw GameEx("EvalState::defense_kings", "invalid color", col);
  
  const set<Loc>& locs = Board::valid_locs();
  int total = 0;
  int enemy_kings = 0;
  int enemy_king_val = (col == IS_RED) ? WHITE_KING : RED_KING;

  for( set<Loc>::iterator i = locs.begin(); i != locs.end(); i++ ) {
    int loc_val = b.get(*i);
    if( loc_val & col ) {
      const Loc l(*i);
      if( l.y == 0 &&  l.y == BOARD_HEIGHT-1 )
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
  
  float max_val = max_value(state, depth, -INFINITY, INFINITY, _color);
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
  
  float max_v = -INFINITY;
  float tmp_v;
  Board tmp_b;
  set<Action>::iterator max_a;
  
  set<Action>& actions = possible_actions(state, color);
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
  
  float min_v = INFINITY;
  float tmp_v;
  Board tmp_b;
  set<Action>::iterator min_a;
  
  set<Action>& actions = possible_actions(state, color);
  set<Action>::iterator itr;
  
  /*
  cout << "--possible actions(min): " << actions.size() << endl;
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
    if( tmp_v < min_v ) {
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

set<Action>& Minimax::possible_actions(const Board& s, int color) const {
  set<Action>& actions = *new set<Action>();
  set<Loc, less<Loc> > movables;
  bool jumps = s.get_move_candidates(movables, color);

  if( !jumps ) {
    set<Loc, less<Loc> >::iterator from_itr = movables.begin();
    for( ; from_itr != movables.end(); from_itr++ ) {  //itr through mvbls
      set<Loc>& neighbours = Board::get_neighbours(*from_itr);

      set<Loc>::iterator to_itr = neighbours.begin();
      for( ; to_itr != neighbours.end();  to_itr++ ) {
	list<Loc> move = list<Loc>();
	if( s.eval_move( Move(*from_itr, *to_itr), color ) != INVALID ) {
	  move.push_back(*from_itr);
	  move.push_back(*to_itr);
	  actions.insert(move);
	}
      }
      delete &neighbours;
    }
  }
  else {     // if( jump )
    set<Loc>::iterator from_itr = movables.begin();       //locs that can move
    for( ; from_itr != movables.end(); from_itr++ ) {
      set<Action> moves_from = get_jump_moves_from(*from_itr, s);

      //move sequences from *from_itr position
      set<Action>::iterator i = moves_from.begin();
      for( ; i != moves_from.end(); i++ ) {
	//(*i).push_front(*from_itr);
	Action tmp_action = Action(*i);
	tmp_action.push_front(*from_itr); //tmp_action.push_back(*from_itr);
	//Action::const_iterator l_itr = (*i).begin();
	//for( ; l_itr != (*i).end(); l_itr++ ) {
	//tmp_action.push_back(*l_itr);
	//}
	actions.insert(tmp_action);
      }
    }
  }

  return actions;
}

set<Action> Minimax::get_jump_moves_from(const Loc& from, const Board& b) const{
  set<Action> actions;
  int color = (b.get(from) & IS_RED) ? IS_RED : IS_WHITE;
  if( b.can_jump_from(from) ) {
    set<Loc>& neigh = Board::get_jump_neighbours(from);
    set<Loc>::iterator to_itr = neigh.begin();
    for( ; to_itr != neigh.end(); to_itr++ ) {    //iterate over neighbours
      int move_bits = b.eval_move( Move(from, *to_itr), color );
      if( move_bits == INVALID ) { 
	continue; 
      }
      
      if( move_bits & KINGED ) {
	Action a = Action();
	a.push_back(*to_itr);
	//cout << "--inserting2: " << *a.begin();
	//cout << " to " << *(--a.end()) << endl;
	actions.insert(a);
      }
      else {  //not kinged
	Board new_board = Board(b);
	new_board.jump_piece( Move(from, *to_itr) );
	
	set<Action> further_actions = get_jump_moves_from(*to_itr, new_board);
	if( further_actions.size() > 0 ) {
	  set<Action>::iterator itr = further_actions.begin();
	  for( ; itr != further_actions.end(); itr++ ) {
	    Action a = Action();
	    a.push_back(*to_itr);

	    Action::const_iterator loc_itr = (*itr).begin();
	    for( ; loc_itr != (*itr).end(); loc_itr++ )
	      a.push_back(*loc_itr);
	    //*/
	    actions.insert(a);
	  }
	}
	else {     //no further actions
	  Action a = Action();
	  a.push_back(*to_itr);
	  actions.insert(a);
	}
      }
    }
    delete &neigh;
  }
  return actions;
}

int Minimax::_other_color(int color) { 
  if( color == EMPTY ) int color = _color;
  return (color == IS_RED) ? IS_WHITE : IS_RED;
}

float Minimax::_eval_state(const Board& b, int c) {
  if( c == EMPTY ) c = _color;
  return _eval_state_func_ptr(b, c);
}
