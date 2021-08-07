/*
 * sharedlock.h
 *		 Created on: 2021-8-7
 *			 Author: qianqians
 * sharedlock:
 */

#ifndef _SHAREDLOCK_H
#define _SHAREDLOCK_H

#include <vector>

#include <spinlock.h>

namespace concurrent {

class NotSharedLockOwnerException : std::exception {
public:
    NotSharedLockOwnerException() : std::exception("unlock thread not owner shared lock!") {
    }
};

class NotUniqueLockOwnerException : std::exception {
public:
    NotUniqueLockOwnerException() : std::exception("unlock thread not owner unique lock!") {
    }
};

class sharedlock {
private:
	spinlock _lock;
	std::vector<std::thread::id> _shared_flag;
	std::thread::id _unique_flag;
    std::thread::id _detail;

public:
	sharedlock() {}
	virtual ~sharedlock() {}

public:
    void lock_shared() {
        while (!try_lock_shared()) {
            std::this_thread::yield();
        }
    }
    bool try_lock_shared() {
        _lock.lock();

        bool lock_state = false;
        do {
            if (_unique_flag != _detail) {
                break;
            }

            auto th_id = std::this_thread::get_id();
            if (std::find(_shared_flag.begin(), _shared_flag.end(), th_id) == _shared_flag.end()) {
                _shared_flag.emplace_back(th_id);
            }
            lock_state = true;

        } while (false);

        _lock.unlock();

        return lock_state;
    }
    void unlock_shared() {
        _lock.lock();

        auto th_id = std::this_thread::get_id();
        auto shared_flag_it = std::find(_shared_flag.begin(), _shared_flag.end(), th_id);
        if (shared_flag_it == _shared_flag.end()) {
            throw NotSharedLockOwnerException();
        }
        _shared_flag.erase(shared_flag_it);

        _lock.unlock();
    }

    void lock_unique() {
        while (!try_lock_unique()) {
            std::this_thread::yield();
        }
    }
    bool try_lock_unique() {
        _lock.lock();

        bool lock_state = false;
        do {
            if (_unique_flag != _detail) {
                break;
            }

            if (!_shared_flag.empty()) {
                break;
            }

            _unique_flag = std::this_thread::get_id();
            lock_state = true;

        } while (false);

        _lock.unlock();

        return lock_state;
    }
    void unlock_unique() {
        _lock.lock();

        auto th_id = std::this_thread::get_id();
        if (_unique_flag != th_id) {
            throw NotUniqueLockOwnerException();
        }
        _unique_flag = _detail;

        _lock.unlock();
    }

    void lock_upgrade() {
        while (true) {
            _lock.lock();

            bool lock_state = false;
            do {
                if (_unique_flag != _detail) {
                    break;
                }

                auto th_id = std::this_thread::get_id();
                auto shared_flag_it = std::find(_shared_flag.begin(), _shared_flag.end(), th_id);
                if (_shared_flag.empty() || (_shared_flag.size() == 1 && shared_flag_it != _shared_flag.end())) {
                    _unique_flag = th_id;
                    lock_state = true;
                }

            } while (false);

            _lock.unlock();

            if (lock_state) {
                break;
            }
            else {
                std::this_thread::yield();
            }
        }
    }
    void unlock_upgrade_and_lock_shared(){
        _lock.lock();

        auto th_id = std::this_thread::get_id();
        if (_unique_flag != th_id) {
            throw NotUniqueLockOwnerException();
        }
        _unique_flag = _detail;

        auto shared_flag_it = std::find(_shared_flag.begin(), _shared_flag.end(), th_id);
        if (shared_flag_it == _shared_flag.end()) {
            _shared_flag.emplace_back(th_id);
        }

        _lock.unlock();
    }
    void unlock_upgrade() {
        _lock.lock();

        auto th_id = std::this_thread::get_id();
        if (_unique_flag != th_id) {
            throw NotUniqueLockOwnerException();
        }
        _unique_flag = _detail;

        auto shared_flag_it = std::find(_shared_flag.begin(), _shared_flag.end(), th_id);
        if (shared_flag_it != _shared_flag.end()) {
            _shared_flag.erase(shared_flag_it);
        }

        _lock.unlock();
    }

};

} /* concurrent */
#endif //_SHAREDLOCK_H