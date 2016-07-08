/*
 * MemoryProfiled.h
 *
 *  Created on: May 23, 2016
 *      Author: manu
 */

#ifndef PLATFORM_MEMORYPROFILED_H_
#define PLATFORM_MEMORYPROFILED_H_

//#define MEM_PROF 1

#include <iostream>
#include <typeinfo>

template<class T>
class MemoryProfiled {
	static int instance_count;
public:
	MemoryProfiled() {
		instance_count++;
	}
	virtual ~MemoryProfiled() {
		instance_count--;
	}
	static void print(size_t &total) {
		std::cout << instance_count << " instances of " << typeid(T).name()
				<< ", " << sizeof(T) << " bytes each totaling "
				<< (instance_count * sizeof(T) / 1000.0) << " Kb." << std::endl;
		total +=  (instance_count * sizeof(T));
	}
};

template<class T>
int MemoryProfiled<T>::instance_count = 0;

#endif /* PLATFORM_MEMORYPROFILED_H_ */
