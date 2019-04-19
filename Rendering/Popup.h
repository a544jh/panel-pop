/*
 * ScorePanel.h
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#ifndef POPUP_H_
#define POPUP_H_

#include <string>

#include "Particle.h"

class Popup : public Particle {
 public:
  Popup();
  Popup(int x, int y, int value, int lifetime);

  int _value;
  std::string _digits;

  virtual void tick();
  virtual void render() = 0;
  virtual ~Popup();
 private:
  void initDigits();
};

#endif /* POPUP_H_ */
