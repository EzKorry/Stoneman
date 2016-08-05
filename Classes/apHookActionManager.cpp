/*
 * apHookActionManager.cpp
 *
 *  Created on: 2015. 9. 20.
 *      Author: ±Ë≈¬»∆
 */

#include "apHookActionManager.h"
#include <cocos2d.h>
#include <sstream>

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


/*
std::shared_ptr<apHookActionManager> apHookActionManager::runHook(const std::string& hook) {

	
}*/

// remove hook.
/*std::shared_ptr<apHookActionManager> apHookActionManager::removeHook(const std::string& hook) {

	// if hook found,
	if(_actions.find(hook) != _actions.end()) {

		_actions.erase(hook);
	}
	return _sp;

}*/

// remove Action.
/*
std::shared_ptr<apHookActionManager> apHookActionManager::removeAction(const std::string& hook, const std::string& tag) {
	auto actionIt = _actions.find(hook);
	if(actionIt != _actions.end()) {
		auto& map = (*actionIt).second;
		auto funcIt = map.find(tag);
		if(funcIt != map.end()) {
			map.erase(funcIt);
		}
	}
	return _sp;
}
*/
apHookActionManager::apHookActionManager()  = default;

std::string apHookActionManager::_d_all_hook()
{/*
	stringstream ss;
	for (auto& item : _actions) {
		ss << item.first << ", ";
	}*/
	return std::string("");
}

apHookActionManager::~apHookActionManager() = default;

apActionFunc::apActionFunc()
{
}

apActionFunc::~apActionFunc()
{
}

 } /* namespace arphomod */


