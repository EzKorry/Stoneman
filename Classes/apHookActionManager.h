/*
 * apHookActionManager.h
 *
 *  Created on: 2015. 9. 20.
 *      Author: ±Ë≈¬»∆
 */

#ifndef APHOOKACTIONMANAGER_H_
#define APHOOKACTIONMANAGER_H_
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <set>

namespace arphomod {
using namespace std;
using apActionContainer = unordered_map<string, function<void()>>;
using apHookActionContainer = unordered_map<string,apActionContainer>;

class apHookActionManager {
public:
	apHookActionManager();

	// add event hook.
	template<class TString>
	shared_ptr<apHookActionManager> addHook(TString&& hook) {

		// if hook not found,
		if(_actions.find(hook) == _actions.end()) {
			_actions.emplace(std::forward<TString>(hook), apActionContainer{});
		}
		return _sp;
	}

	// add function to hook.
	template<class TFunc>
	shared_ptr<apHookActionManager> addAction(const std::string& hook, const std::string& tag, TFunc&& action) {


		addHook(hook);

		// if tag not found, ignore.
		_actions[hook].emplace(tag, std::forward<TFunc>(action));
		return _sp;

	}
	// run hook. then all that function will be invoked.
	shared_ptr<apHookActionManager> runHook(const std::string& hook);

	// remove hook.
	shared_ptr<apHookActionManager> removeHook(const std::string& hook);

	// remove Action.
	shared_ptr<apHookActionManager> removeAction(const std::string& hook, const string& tag);

	// singleton.
	static shared_ptr<apHookActionManager> getInstance() {

		if(_sp == nullptr) {
			_sp = make_shared<apHookActionManager>();
		}
		return _sp;
	}

	// for debug.
	std::string _d_all_hook();

	virtual ~apHookActionManager();

private:
	string _defaultTag {"empty"};
	// key: hook.
	// value: key: actionTag.
	//        value: action function
	apHookActionContainer _actions;

	static shared_ptr<apHookActionManager> _sp;
};

} /* namespace arphomod */

#endif /* APHOOKACTIONMANAGER_H_ */
