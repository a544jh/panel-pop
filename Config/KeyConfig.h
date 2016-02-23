/*
 * KeyboardControllerConfig.h
 *
 *  Created on: Dec 17, 2015
 *      Author: axel
 */

#ifndef CONFIG_KEYCONFIG_H_
#define CONFIG_KEYCONFIG_H_

#define KEYS \
	X(up)\
	X(down)\
	X(left)\
	X(right)\
	X(swap)\
	X(raiseStack)

struct KeyConfig {
#define X(key) int key;
	KEYS
#undef X
};

#endif /* CONFIG_KEYCONFIG_H_ */
