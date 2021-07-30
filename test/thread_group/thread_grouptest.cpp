/*
 * thread_grouptest.cpp
 *  Created on: 2021-7-30
 *	    Author: qianqians
 * thread_group test
 */
#include <thread>
#include <iostream>
#include <vector>

#include <thread_group.h>

int main(){
	concurrent::spinlock l;
	concurrent::thread_group th_group;

	for (auto i = 0; i < 100; i++) {
		th_group.create_thread([&l]() {
			l.lock();
			std::cout << std::this_thread::get_id() << std::endl;
			l.unlock();
		});
	}

	th_group.join_all();

	char n;
	std::cin >> n;

	return 1;
}