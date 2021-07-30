/*
 * msquebasetest.cpp
 *  Created on: 2013-8-17
 *	    Author: qianqians
 * msque»ù×¼²âÊÔ
 */
#include <thread>
#include <iostream>
#include <vector>

#include <spinlock.h>

int main(){
	lock_free::spinlock l;

	std::vector<std::thread*> ths;
	for (auto i = 0; i < 100; i++) {
		ths.push_back(new std::thread([&l]() {
			l.lock();
			std::cout << std::this_thread::get_id() << std::endl;
			l.unlock();
		}));
	}

	for (auto th : ths) {
		th->join();
	}

	char n;
	std::cin >> n;

	return 1;
}