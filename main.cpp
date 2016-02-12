/*
 * main.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#include "Config/ConfigHandler.h"
#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <iostream>


int main(int argc, char* args[]) {
	//srand(time(NULL));
	//SDLContext& SDL = SDLContext::getInstance();
	ConfigHandler& configHandler = ConfigHandler::getInstance();

	try {
		configHandler.loadConfig();
	} catch(boost::exception& ex) {
		std::cout << boost::diagnostic_information(ex);
	}

	//SDL.init();
	//StateManager& sm = StateManager::getInstance();
	//sm.run();

	//SDL.tearDown();

//configHandler.saveConfig();

	return 0;
}
