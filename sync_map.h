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
	template <typename V>
	struct entry {
		std::atomic<V*> p;
	};

	struct read_only {
		std::shared_ptr<std::map<K, std::shared_ptr<entry<V>>>> m = std::make_shared<std::map<K, std::shared_ptr<entry<V>>>>();
		bool amended = false;
	};

	typedef concurrent::detail::_hazard_ptr<read_only> _hazard_ptr;
	typedef concurrent::detail::_hazard_system<read_only> _hazard_system;
		
public:
	sync_map() : _hazard_sys(std::bind(&sync_map::put_read_only, this, std::placeholders::_1)) {
		_read.store(get_read_only());
		_dirty = std::make_shared<std::map<K, std::shared_ptr<entry<V>>>>();
		misses = 0;
	}

	virtual ~sync_map() {
		read_only* _p = _read.load();

		if (_dirty) {
			for (auto [k, e] : *(_dirty)) {
				delete e->p.load();
			}
		}
		else {
			for (auto [k, e] : *(_p->m)) {
				delete e->p.load();
			}
		}

		if (_p) {
			put_read_only(_p);
		}
	}

private:
	read_only* get_read_only(){
		return new read_only();
	}

	void put_read_only(read_only* _p){
		delete _p;
	}

private:
	std::mutex _mu;
	std::atomic<read_only*> _read;
	std::shared_ptr<std::map<K, std::shared_ptr<entry<V>>>> _dirty;
	int misses;

	_hazard_system _hazard_sys;

};

} /* concurrent */
#endif //_SYNC_MAP_H