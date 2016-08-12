#include "apAsyncTaskManager.h"
#include <chrono>


std::shared_ptr<apAsyncTaskManager> apAsyncTaskManager::_sp = nullptr;
apAsyncTaskManager::apAsyncTaskManager()
{
	init();
}


void apAsyncTaskManager::addTask(apTaskCallType&& callback)
{
	//if (std::find(_list.begin(), _list.end(), callback.) != _list.end()) {
		_list.emplace_back(std::move(callback));
	//}
}

/*void apAsyncTaskManager::addTask(const function<void()>& callback)
{
}*/

apTaskCallType & apAsyncTaskManager::getTask()
{
	return _doTasks;
}

void apAsyncTaskManager::init() {
	_doTasks = boost::coroutines::symmetric_coroutine<void>::call_type(
		[&, this](boost::coroutines::symmetric_coroutine<void>::yield_type& yield) {
		auto elapsedTime = chrono::duration<float>::zero();
		auto firstStartTime = chrono::steady_clock::now();
		auto startTime = firstStartTime;
		//cocos2d::log("entered do_tasks");
		for (;;) {
			if (_list.empty()) {
				yield();
				//cocos2d::log("listEmpty!");
			}
			else {
				startTime = chrono::steady_clock::now();
				elapsedTime = chrono::duration<float>::zero();
				while (!_list.empty() && elapsedTime.count() < _delta) {
					if (_list.front()) {
						//yield(_list.front());
						_list.front()();
					} else {
						_list.pop_front();
						//cocos2d::log("listpopfront");
					}
					elapsedTime += chrono::steady_clock::now() - startTime;
					startTime = chrono::steady_clock::now();
					//cocos2d::log("elapsedTimeAdded -- %f", elapsedTime.count());
				}
				//cocos2d::log("elapsedTimeOver");
				cocos2d::log("elapsedTime: %f", elapsedTime.count());
				yield();

			}
		}
	
	});

}

void apAsyncTaskManager::setInterval(float delta)
{
	_delta = delta;
}

void apAsyncTaskManager::doTasks()
{
	try {
		_doTasks();
	} catch(const boost::coroutines::detail::forced_unwind&) {
		throw;
	}
	catch (...) {
		// possibly not re-throw pending exception
	}
}

apAsyncTaskManager::~apAsyncTaskManager()
{
}
