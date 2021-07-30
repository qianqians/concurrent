/*
 * spinlocktest.cpp
 *  Created on: 2021-7-30
 *	    Author: qianqians
 * spinlocktest
 */
#include <thread>
#include <iostream>
#include <vector>

#include <spinlock.h>

int main(){
	concurrent::spinlock l;

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