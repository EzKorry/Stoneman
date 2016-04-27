/*
 * apHookActionManager.cpp
 *
 *  Created on: 2015. 9. 20.
 *      Author: ±Ë≈¬»∆
 */

#include "apHookActionManager.h"
#include <cocos2d.h>

namespace arphomod {
/*
template<class TString>
void apHookActionManager::addHook(TString&& hook) {

	// if hook not found,
	if(_actions.find(hook) == _actions.end()) {
		_actions.emplace(std::forward<TString>(hook), std::set<string>());
	}
}

template<class TFunc>
void apHookActionManager::addAction(const std::string& hook, const std::string& tag, TFunc&& action) {


	addHook(hook);

	// if tag not found,
	_actions[hook].emplace(tag);
	_tagToFunc.emplace(tag, std::forward<TFunc>(action));

}*/

// run hook. then all that function will be invoked.
std::shared_ptr<apHookActionManager> apHookActionManager::_sp = nullptr;



void apHookActionManager::runHook(const std::string& hook) {

	// if hook found,
	if(_actions.find(hook) != _actions.end()) {

		// run each function.
		for(auto& item : _actions[hook]) {
			////cocos2d::log("actionManager RunHook string:%s",hook.c_str());

			item.second();
		}
	}
}

// remove hook.
void apHookActionManager::removeHook(const std::string& hook) {

	// if hook found,
	if(_actions.find(hook) != _actions.end()) {

		_actions.erase(hook);
	}

}

// remove Action.
void apHookActionManager::removeAction(const std::string& hook, const std::string& tag) {
	auto actionIt = _actions.find(hook);
	if(actionIt != _actions.end()) {
		auto& map = (*actionIt).second;
		auto funcIt = map.find(tag);
		if(funcIt != map.end()) {
			map.erase(funcIt);
		}
	}
}



apHookActionManager::apHookActionManager()  = default;

apHookActionManager::~apHookActionManager() = default;





} /* namespace arphomod */
