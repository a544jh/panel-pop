/*
 * OptionsMenu.cpp
 *
 *  Created on: 22.2.2016
 *      Author: axelw
 */

#include "OptionsMenu.h"

OptionsMenu::OptionsMenu() {
	addItem(MenuItem("Fullscreen", [&](){},0,0));
	addItem(MenuItem("Music volume", [&](){},0,0));
	addItem(MenuItem("SFX volume", [&](){},0,0));
	addItem(MenuItem("Configure keys", [&](){},0,0));
}

OptionsMenu::~OptionsMenu() {
}

