/*
 * ChainPanel.h
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#ifndef CHAINPOPUP_H_
#define CHAINPOPUP_H_

#include "Popup.h"

class ChainPopup : public Popup {
 public:
  ChainPopup();
  virtual ~ChainPopup();
  ChainPopup(int x, int y, int value, int lifetime);
  void render();
};

#endif /* CHAINPOPUP_H_ */
