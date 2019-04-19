/*
 * Game.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef GAME_H_
#define GAME_H_

#include "Board.h"
#include "Game.h"

class GameEventHandler;

class VsGame : public Game {
 public:

  VsGame();

  static const int MATCH_POINTS = 2;

  virtual ~VsGame();

  void reset();
  void tick();

  int getP1MatchPoints() const;
  int getP1Points() const;
  int getP2MatchPoints() const;
  int getP2Points() const;
  Board &getBoard(int id);

 private:

  Board _board0;
  Board _board1;

  int _p1MatchPoints;
  int _p2MatchPoints;
  int _p1Points;
  int _p2Points;

  void handleEnd();

  void handleGarbageSpawning(Board &, Board &);
};

#endif /* GAME_H_ */
