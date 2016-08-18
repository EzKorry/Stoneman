#ifndef _APDETACHMANAGER_H_
#define _APDETACHMANAGER_H_

#include <cocos2d.h>
#include <map>
#include <set>
#include <memory>
namespace arphomod {

enum class apDetachManagerType {
	TouchManager,
	HookActionManager,
	AnimationManager,
	AsyncTaskManager

};
/*
template<apDetachManagerType T>
class apDetachManagerT {
	using type = void*;
};

template<>
class apDetachManagerT<apDetachManagerType::TouchManager> {
	using type = APTouchManager;
	static std::set<cocos2d::Node*> nodes;
};

template<>
class apDetachManagerT<apDetachManagerType::HookActionManager> {
	using type = apHookActionManager;
};

template<>
class apDetachManagerT<apDetachManagerType::AnimationManager> {
	using type = apAnimationManager;
};

template<>
class apDetachManagerT<apDetachManagerType::AsyncTaskManager> {
	using type = apAsyncTaskManager;
};*/

class apDetachInterface;
class apDetachManager
{
public:
	apDetachManager();
	virtual ~apDetachManager();

	void addNode(cocos2d::Node* node);
	void addNode(cocos2d::Node* node, apDetachManagerType t);
	void detach(cocos2d::Node* node);
	void addScheduler(cocos2d::Scheduler* scheduler);
	void addActionManager(cocos2d::ActionManager* actionManager);
	static std::shared_ptr<apDetachManager> getInstance();

private:
	std::map<cocos2d::Node*, std::set<apDetachManagerType>> _detachList;
	std::map<apDetachManagerType, std::shared_ptr<apDetachInterface>> _managers;
	std::set<cocos2d::Scheduler*> _schedulers;
	std::set<cocos2d::ActionManager*> _actionManagers;
	static std::shared_ptr<apDetachManager> _sp;
	

};

class apDetachInterface {
public:
	virtual void detach(cocos2d::Node* node) = 0;
};
}
#endif