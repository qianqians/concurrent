/*
 * sync_maptest.cpp
 *  Created on: 2025-4-20
 *	    Author: qianqians
 * sync_map test
 */
#include <thread>
#include <iostream>
#include <vector>

#include <sync_map.h>
#include <thread_group.h>

int main(){
	std::mutex _l_th_mu;
	concurrent::thread_group th_group;

	concurrent::sync_map<std::string, int32_t> _sync_map;

	for (auto i = 0; i < 100; i++) {
		th_group.create_thread([&_l_th_mu]() {
			std::lock_guard<std::mutex> l(_l_th_mu);
			std::cout << std::this_thread::get_id() << std::endl;
		});
	}

	th_group.join_all();

	char n;
	std::cin >> n;

	return 1;
}