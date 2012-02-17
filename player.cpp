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
