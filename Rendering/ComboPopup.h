/*
 * ComboPopup.h
 *
 *  Created on: Oct 25, 2015
 *      Author: axel
 */

#ifndef COMBOPOPUP_H_
#define COMBOPOPUP_H_

#include "Popup.h"

class ComboPopup : public Popup {
 public:
  ComboPopup();
  ComboPopup(int x, int y, int value, int lifetime);
  virtual ~ComboPopup();
  void render();
};

#endif /* COMBOPOPUP_H_ */
