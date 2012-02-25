#include "player.h"

Player::Player(const string& n, const int colour) : name(n), color(colour)
{  }
Player::Player(const string& n, const Board& b, const int colour) : 
  name(n), board(b), color(colour)
{  }

void Player::set_board(const Board& b) {
  board = Board(b);
}

const string& Player::get_name() {
  return name;
}

const int Player::get_color(void) {
  return color;
}

set<Action>& Player::possible_actions(const Board& s, int color) {
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
      set<Action>& moves_from = get_jump_moves_from(*from_itr, s);

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
      delete &moves_from;
    }
  }

  return actions;
}

set<Action>& Player::get_jump_moves_from(const Loc& from, const Board& b) {
  set<Action>& actions = *new set<Action>();
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
	
	set<Action>& further_actions = get_jump_moves_from(*to_itr, new_board);
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
	delete &further_actions;
      }
    }
    delete &neigh;
  }
  return actions;
}
