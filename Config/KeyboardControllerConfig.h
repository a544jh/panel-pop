/*
 * KeyboardControllerConfig.h
 *
 *  Created on: Dec 17, 2015
 *      Author: axel
 */

#ifndef CONFIG_KEYBOARDCONTROLLERCONFIG_H_
#define CONFIG_KEYBOARDCONTROLLERCONFIG_H_

#define KEYS \
	X(up)\
	X(down)\
	X(left)\
	X(right)\
	X(swap)\
	X(raiseStack)

struct KeyboardControllerConfig {
#define X(key) int key;
	KEYS
#undef X
};

#endif /* CONFIG_KEYBOARDCONTROLLERCONFIG_H_ */
