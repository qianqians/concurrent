/*
 * sync_map.h
 *		 Created on: 2025-4-20
 *			 Author: qianqians
 * sync_map:
 */

#ifndef _SYNC_MAP_H
#define _SYNC_MAP_H

#include <map>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>

#include "./detail/_hazard_ptr.h"

namespace concurrent {

template <typename K, typename V>
class sync_map {
private:
	struct read_only {
		std::shared_ptr<std::map<K, V>> m;
		bool amended;
	};

	typedef concurrent::detail::_hazard_ptr<read_only> _hazard_ptr;
	typedef concurrent::detail::_hazard_system<read_only> _hazard_system;
		
public:
	sync_map() : _hazard_sys(std::bind(&sync_map::put_read_only, this, std::placeholders::_1)) {
		_read.store(get_read_only());
		_dirty = std::make_shared<std::map<K, V>>();
		misses = 0;
	}

private:
	read_only* get_read_only(){
		read_only* __r = new read_only();

		__r->m = std::make_shared<std::map<K, V>>();
		__r->amended = false;

		return __r;
	}

	void put_read_only(read_only* _p){
		_hazard_sys.retire(_p);
	}

private:
	std::mutex _mu;
	std::atomic<read_only*> _read;
	std::shared_ptr<std::map<K, V>> _dirty;
	int misses;

	_hazard_system _hazard_sys;

};

} /* concurrent */
#endif //_SYNC_MAP_H