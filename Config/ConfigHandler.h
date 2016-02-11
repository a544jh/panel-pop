/*
 * ConfigHandler.h
 *
 *  Created on: 11.2.2016
 *      Author: axelw
 */

#ifndef CONFIG_CONFIGHANDLER_H_
#define CONFIG_CONFIGHANDLER_H_

#include<boost/property_tree/ptree.hpp>

class ConfigHandler {
public:

	static ConfigHandler& getInstance();

	static constexpr const char* CONFIG_FILENAME = "panelpop.ini";

	bool loadConfig();
	bool saveConfig();

private:
	ConfigHandler();
	ConfigHandler(ConfigHandler const&) = delete;
	void operator=(ConfigHandler const&) = delete;

	boost::property_tree::ptree _settingsTree;
};

#endif /* CONFIG_CONFIGHANDLER_H_ */
