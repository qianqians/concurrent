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
	concurrent::thread_group th_group;
	concurrent::sync_map<std::string, int32_t> _sync_map;

	for (auto i = 0; i < 100; i++) {
		th_group.create_thread([&_sync_map, i]() {
			_sync_map.store(std::format("key_{0}", i), i);
			int num = 0;;
			_sync_map.load(std::format("key_{0}", i), num);
			std::cout << std::this_thread::get_id() << std::endl;
		});
	}

	th_group.join_all();

	_sync_map.range([](auto k, auto v) {
		std::cout << k << ":" << v << std::endl;
		return true;
	});

	for (auto i = 0; i < 100; i++) {
		_sync_map.erase(std::format("key_{0}", i));
	}

	char n;
	std::cin >> n;

	return 1;
}