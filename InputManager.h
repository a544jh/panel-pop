/*
 * InputManager.h
 *
 *  Created on: Aug 19, 2015
 *      Author: axel
 */

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <vector>
#include <SDL_joystick.h>

class InputManager {
 public:
  static InputManager &getInstance();

  void poll();
  void detectJoysticks();
  SDL_Joystick *getJoystick(SDL_JoystickID id);

  bool _quit;
  int _keys_len;
  const uint8_t *_keys;
  uint8_t *_prevKeys;

  bool keyDown(int); // key pressed down this poll
  bool anyKeyDown();

  bool keyUp(int);

  bool keyPressed(int); // key is down now
  int getKeyDown();

 private:
  InputManager();

  InputManager(InputManager const &) = delete;

  void operator=(InputManager const &) = delete;

  std::vector<SDL_Joystick*> _joysticks;
};

#endif /* INPUTMANAGER_H_ */
