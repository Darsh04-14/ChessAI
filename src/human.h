#ifndef HUMAN_H
#define HUMAN_H

#include "chess.h"
#include "player.h"

class Human : public Player {
 public:
  Human(Chess *g) : Player{g} {}
  bool notify() override;
  ~Human();
};

#endif
