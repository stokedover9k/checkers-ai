#ifndef __CH_EXCEPTIONS_
#define __CH_EXCEPTIONS_

#include <string>
#include <iostream>

using namespace std;

class GameEx {
 public:
  GameEx(string origin, string message, int valA=0, int valB=0);
  
  void add_trace(string s);
  
  string orgn, msg;
  int a, b;
  
  friend ostream& operator << (ostream&, GameEx&);
};
///////////////////////////////////////////////////////////////////////////////

#endif
