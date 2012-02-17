#include "exceptions.h"

GameEx::GameEx(string o, string mes, int va, int vb) :
  orgn(o), msg(mes), a(va), b(vb)
{  }

void GameEx::add_trace(string s) {
  orgn = orgn + " -" + s;
}

ostream& operator << (ostream& s, GameEx& e) {
  s << e.orgn << ": " << e.msg << ": (" << e.a << ", " << e.b << ")";
  return s;
}
///////////////////////////////////////////////////////////////////////////////
