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

class TheSpinLockIsUnlockException : std::exception {
public:
	TheSpinLockIsUnlockException() : std::exception("the lock is unlock!") {
	}
};

class NotSpinLockOwnerThreadException : std::exception {
public:
	NotSpinLockOwnerThreadException() : std::exception("unlock thread not owner thread!") {
	}
};

class spinlock {
private:
	std::atomic<std::thread::id> _flag;

public:
	spinlock() {}
	virtual ~spinlock() {}

private:
	bool try_lock(std::thread::id curr_th_id) {
		if (_flag.load() == curr_th_id) {
			return true;
		}

		std::thread::id detail;
		return _flag.compare_exchange_strong(detail, curr_th_id);
	}

public:
	bool try_lock() {
		auto th_id = std::this_thread::get_id();
		return try_lock(th_id);
	}

	void lock() {
		auto th_id = std::this_thread::get_id();
		std::thread::id detail;
		while (!try_lock(th_id)) {
			while (1) {
				_mm_pause();

				if (_flag.load() == detail) {
					break;
				}

				std::this_thread::yield();

				if (_flag.load() == detail) {
					break;
				}
			}
		}
	}

	void unlock() {
		std::thread::id detail;
		if (_flag.load() == detail) {
			throw TheSpinLockIsUnlockException();
		}

		auto th_id = std::this_thread::get_id();
		if (th_id != _flag.load()) {
			throw NotSpinLockOwnerThreadException();
		}

		_flag.store(std::thread::id());
	}

};

} /* concurrent */
#endif //_SPINLOCK_H