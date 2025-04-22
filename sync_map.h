/*
 * sync_map.h
 * fork from go sync.map
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
		static inline const V* const expunged = new V();

		std::atomic<V*> p;

		virtual ~entry() {
			if (p.load()) {
				delete p.load();
			}
		}

		bool load(V &value) {
			auto _p = p.load();
			if (_p == nullptr || _p == entry::expunged) {
				return false;
			}
			value = *_p;
			return true;
		}

		bool unexpungeLocked() {
			auto _p = (V*)entry::expunged;
			return p.compare_exchange_weak(_p, nullptr);
		}

		void swapLocked(V value) {
			auto vp = new V(value);
			auto old = p.exchange(vp);
			delete old;
		}

		bool trySwap(V value) {
			auto vp = new V(value);
			while(true) {
				auto _p = p.load();
				if (_p == entry::expunged) {
					delete vp;
					return false;
				}
				if (p.compare_exchange_weak(_p, vp)){
					delete _p;
					return true;
				}
			}
		}

		bool _delete() {
			while (true) {
				auto _p = p.load();
				if (_p == nullptr || _p == entry::expunged) {
					return false;
				}
				if (p.compare_exchange_strong(_p, nullptr)) {
					delete _p;
					return true;
				}
			}
		}

		bool tryExpungeLocked() {
			auto _p = p.load();
			while (_p == nullptr) {
				auto _pnull = (V*)nullptr;
				auto _pexpunged = (V*)entry::expunged;
				if (p.compare_exchange_strong(_pnull, _pexpunged)) {
					return true;
				}
				auto _p = p.load();
			}
			return _p == expunged;
		}
	};

	struct read_only {
		std::shared_ptr<std::map<K, std::shared_ptr<entry<V>>>> m;
		bool amended;

		read_only() {
			m = std::make_shared<std::map<K, std::shared_ptr<entry<V>>>>();
			amended = false;
		}

		read_only(std::shared_ptr<std::map<K, std::shared_ptr<entry<V>>>> dirty) {
			m = dirty;
			amended = false;
		}
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

	bool load(K key, V& data) {
		_hazard_ptr* _hptr;
		_hazard_sys.acquire(&_hptr, 1);

		_hptr->_hazard = _read.load();
		auto eit = _hptr->_hazard->m->find(key);
		bool find = eit != _hptr->_hazard->m->end();

		if (!find && _hptr->_hazard->amended) {
			std::unique_lock<std::mutex> l(_mu);

			_hptr->_hazard = _read.load();
			eit = _hptr->_hazard->m->find(key);
			find = eit != _hptr->_hazard->m->end();

			if (!find && _hptr->_hazard->amended) {
				auto deit = _dirty->find(key);
				find = deit != _dirty->end();
				if (find) {
					data = *(deit->second->p);
					missLocked();
					return find;
				}
			}
		}

		if (find) {
			data = *(eit->second->p);
		}

		_hazard_sys.release(_hptr);

		return find;
	}

	void store(K key, V value) {
		_hazard_ptr* _hptr;
		_hazard_sys.acquire(&_hptr, 1);

		_hptr->_hazard = _read.load();
		auto eit = _hptr->_hazard->m->find(key);
		bool find = eit != _hptr->_hazard->m->end();
		if (find) {
			eit->second->trySwap(value);
		}

		std::unique_lock<std::mutex> l(_mu);
		_hptr->_hazard = _read.load();
		eit = _hptr->_hazard->m->find(key);
		find = eit != _hptr->_hazard->m->end();
		if (find) {
			if (eit->second->unexpungeLocked()) {
				_dirty->insert(std::make_pair(key, eit->second));
			}
			eit->second->swapLocked(value);
		}
		else {
			find = false;
			if (_dirty) {
				auto deit = _dirty->find(key);
				find = deit != _dirty->end();
				if (find) {
					deit->second->swapLocked(value);
				}
			}
			if (!find) {
				if (!_hptr->_hazard->amended) {
					dirtyLocked();
					_hptr->_hazard->amended = true;
				}

				auto e = std::make_shared<entry<V>>();
				e->p = new V(value);
				_dirty->insert(std::make_pair(key, e));
			}
		}

		_hazard_sys.release(_hptr);
	}

	void erase(K key) {
		_hazard_ptr* _hptr;
		_hazard_sys.acquire(&_hptr, 1);

		_hptr->_hazard = _read.load();
		auto eit = _hptr->_hazard->m->find(key);
		bool find = eit != _hptr->_hazard->m->end();
		if (!find && _hptr->_hazard->amended) {
			std::unique_lock<std::mutex> l(_mu);

			_hptr->_hazard = _read.load();
			eit = _hptr->_hazard->m->find(key);
			find = eit != _hptr->_hazard->m->end();
			if (!find && _hptr->_hazard->amended) {
				_dirty->erase(key);
				missLocked();
			}
		}
		else {
			eit->second->_delete();
		}

		_hazard_sys.release(_hptr);
	}

	void range(std::function<bool(K key, V value)> fn) {
		_hazard_ptr* _hptr;
		_hazard_sys.acquire(&_hptr, 1);

		_hptr->_hazard = _read.load();
		if (_hptr->_hazard->amended) {
			std::unique_lock<std::mutex> l(_mu);

			_hptr->_hazard = _read.load();
			if (_hptr->_hazard->amended) {
				auto new_r = new read_only();
				new_r->m = _dirty;

				_read.store(new_r);
				_hazard_sys.retire(_hptr->_hazard);

				_hptr->_hazard = _read.load();

				_dirty = nullptr;
				misses = 0;
			}
		}

		for (auto [k, e] : *(_hptr->_hazard->m)) {
			V value;
			if (!e->load(value)) {
				continue;
			}
			if (!fn(k, value)) {
				break;
			}
		}
	}

private:
	void dirtyLocked() {
		if (_dirty != nullptr) {
			return;
		}

		_hazard_ptr* _hptr;
		_hazard_sys.acquire(&_hptr, 1);
		_hptr->_hazard = _read.load();

		_dirty = std::make_shared<std::map<K, std::shared_ptr<entry<V>>>>();
		for (auto [k, e] : *(_hptr->_hazard->m)) {
			if (!e->tryExpungeLocked()) {
				_dirty->insert(std::make_pair(k, e));
			}
		}

		_hazard_sys.release(_hptr);
	}

	void missLocked() {
		misses++;
		if (misses < _dirty->size()) {
			return;
		}
		auto new_r = new read_only(_dirty);

		_hazard_ptr* _hptr;
		_hazard_sys.acquire(&_hptr, 1);

		while (true) {
			_hptr->_hazard = _read.load();
			if (_read.compare_exchange_strong(_hptr->_hazard, new_r)) {
				_hazard_sys.retire(_hptr->_hazard);
				break;
			}
		}

		_hazard_sys.release(_hptr);

		_dirty = nullptr;
		misses = 0;
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