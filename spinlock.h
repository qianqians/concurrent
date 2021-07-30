/*
 * spinlock.h
 *		 Created on: 2021-7-30
 *			 Author: qianqians
 * spinlock:
 */

#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <atomic>
#include <thread>

namespace concurrent {

class spinlock {
private:
	std::atomic_bool _flag;

public:
	spinlock() {}
	virtual ~spinlock() {}

	bool try_lock() {
		return !_flag.exchange(true);
	}

	void lock() {
		while (_flag.exchange(true)) {
			while (1) {
				_mm_pause();

				if (_flag.load() == false) {
					break;
				}

				std::this_thread::yield();

				if (_flag.load() == false) {
					break;
				}
			}
		}
	}

	void unlock() {
		_flag.store(false);
	}

};

} /* concurrent */
#endif //_SPINLOCK_H