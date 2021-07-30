/*
 * thread_group.h
 *		 Created on: 2021-7-30
 *			 Author: qianqians
 * thread_group
 */

#ifndef _THREAD_GROUP_H
#define _THREAD_GROUP_H

#include <memory>
#include <vector>
#include <thread>
#include <functional>

#include <spinlock.h>

namespace concurrent {

class thread_group {
private:
	spinlock _l_th_group;
	std::vector<std::shared_ptr<std::thread> > _th_group;

public:
	thread_group() {}
	virtual ~thread_group() {
		_th_group.clear();
	}

	std::shared_ptr<std::thread> create_thread(std::function<void()> fn) {
		auto th = std::make_shared<std::thread>(fn);

		_l_th_group.lock();
		_th_group.emplace_back(th);
		_l_th_group.unlock();

		return th;
	}

	void add_thread(std::shared_ptr<std::thread> th) {
		_l_th_group.lock();
		_th_group.emplace_back(th);
		_l_th_group.unlock();
	}

	void remove_thread(std::shared_ptr<std::thread> th) {
		_l_th_group.lock();
		auto it = std::find(_th_group.begin(), _th_group.end(), th);
		if (it != _th_group.end()) {
			_th_group.erase(it);
		}
		_l_th_group.unlock();
	}

	void join_all() {
		_l_th_group.lock();
		for (auto th : _th_group) {
			th->join();
		}
		_th_group.clear();
		_l_th_group.unlock();
	}

};

} /* concurrent */
#endif //_THREAD_GROUP_H