/*
 * MenuItem.h
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#ifndef MENUS_MENUITEM_H_
#define MENUS_MENUITEM_H_

#include <functional>
#include <string>
#include <vector>

class Menu;

typedef std::function<void()> command;

class MenuItem {
 public:

  enum class OptionType {
    NONE, SLIDER, TOGGLE, PLAYER, LIST
  };

  MenuItem(std::string, command);
  MenuItem(std::string, command, int value, int max, OptionType type);
  // LIST: _value indexes into the option names shown next to the item.
  MenuItem(std::string, command, int value, int max, OptionType type,
           const std::vector<std::string> &options);
  virtual ~MenuItem();
  command getFn() const;
  const std::string &getName() const;
  const std::string getOptionString() const;
  void increase();
  void decrease();
  OptionType getOptionType() const;
  int getMax() const;
  const std::vector<std::string> &getOptions() const;
  int getValue() const;
  void setValue(int value);

 private:
  OptionType _optionType;
  std::vector<std::string> _options;
  std::string _name;
  command _fn;
  int _value;
  int _max;
};

#endif /* MENUS_MENUITEM_H_ */
