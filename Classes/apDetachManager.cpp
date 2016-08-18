

#include "apDetachManager.h"
#include "apTouchManager.h"
#include "apHookActionManager.h"
#include "apAsyncTaskManager.h"
#include "apAnimationManager.h"

namespace arphomod {

std::shared_ptr<apDetachManager> apDetachManager::_sp = nullptr;
apDetachManager::apDetachManager()
{
}


apDetachManager::~apDetachManager()
{
}

void apDetachManager::addNode(cocos2d::Node * node)
{
	// create empty map element.
	_detachList[node];
	node->setOnExitCallback([node, this]()->void {
		detach(node);
	});
}

void apDetachManager::addNode(cocos2d::Node * node, apDetachManagerType t)
{
	if (_managers.find(t) == _managers.end()) {
		switch (t) {
		case apDetachManagerType::TouchManager:
			_managers[t] = APTouchManager::getInstance();
			break;
		case apDetachManagerType::HookActionManager:
			_managers[t] = apHookActionManager::getInstance();
			break;
		case apDetachManagerType::AnimationManager:
			_managers[t] = apAnimationManager::getInstance();
			break;
		case apDetachManagerType::AsyncTaskManager:
			_managers[t] = apAsyncTaskManager::getInstance();
			break;
		default:
			break;
		}
	}
	
	addNode(node);

	_detachList[node].insert(t);

	
}

void apDetachManager::detach(cocos2d::Node * node)
{
	for (auto& item : _detachList[node]) {
		_managers[item]->detach(node);
	}
	for (auto& scheduler : _schedulers) {
		scheduler->pauseTarget(node);
	}
	for (auto& actionManager : _actionManagers) {
		actionManager->pauseTarget(node);
	}
	_detachList.erase(node);
}
void apDetachManager::addScheduler(cocos2d::Scheduler * scheduler)
{
	if (scheduler) { _schedulers.emplace(scheduler); }
}
void apDetachManager::addActionManager(cocos2d::ActionManager * actionManager)
{
	if (actionManager) { _actionManagers.emplace(actionManager); }
}
std::shared_ptr<apDetachManager> apDetachManager::getInstance()
{
	if (_sp == nullptr) {
		_sp = make_shared<apDetachManager>();
	}
	
	return _sp;
}
}