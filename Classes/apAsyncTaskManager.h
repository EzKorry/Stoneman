#ifndef _APASYNCTASKMANAGER_H_
#define _APASYNCTASKMANAGER_H_

#include <cocos2d.h>
#include <memory>
#include <vector>
#include <boost/coroutine/all.hpp>

using namespace std;
using apTaskType = boost::coroutines::symmetric_coroutine<void>::call_type;
class apAsyncTaskManager
{
public:
	apAsyncTaskManager();
	static std::shared_ptr<apAsyncTaskManager> getInstance() {

		if (_sp == nullptr) {
			_sp = std::make_shared<apAsyncTaskManager>();

		}
		return _sp;
	}
	void addTask(boost::coroutines::symmetric_coroutine<void>::call_type&& callback);
	apTaskType& getTask();
	virtual void init();
	virtual void setInterval(float delta);
	virtual void doTasks();

	virtual ~apAsyncTaskManager();
	
private:
	static std::shared_ptr<apAsyncTaskManager> _sp;
	std::list<apTaskType> _list;
	apTaskType _doTasks;
	float _delta{ 1.f / 60.f };
};

#endif