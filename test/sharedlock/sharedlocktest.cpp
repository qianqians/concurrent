/*
 * spinlocktest.cpp
 *  Created on: 2021-7-30
 *	    Author: qianqians
 * spinlocktest
 */
#include <thread>
#include <iostream>
#include <vector>

#include <sharedlock.h>

int main(){
	concurrent::sharedlock l;
	int flag = 0;

	std::vector<std::thread*> ths;
	for (auto i = 0; i < 100; i++) {
		ths.push_back(new std::thread([&l, &flag]() {
			l.lock_shared();
			std::cout << std::this_thread::get_id() << " primitive flag:" << flag << std::endl;
			l.unlock_shared();
			l.lock_unique();
			flag++;
			std::cout << std::this_thread::get_id() << " new flag:" << flag << std::endl;
			l.unlock_unique();
		}));
	}

	for (auto th : ths) {
		th->join();
	}

	char n;
	std::cin >> n;

	return 1;
}