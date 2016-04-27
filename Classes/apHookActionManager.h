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
	void addHook(TString&& hook) {

		// if hook not found,
		if(_actions.find(hook) == _actions.end()) {
			_actions.emplace(std::forward<TString>(hook), apActionContainer{});
		}
	}

	// add function to hook.
	template<class TFunc>
	void addAction(const std::string& hook, const std::string& tag, TFunc&& action) {


		addHook(hook);

		// if tag not found, ignore.
		_actions[hook].emplace(tag, std::forward<TFunc>(action));

	}
	// run hook. then all that function will be invoked.
	void runHook(const std::string& hook);

	// remove hook.
	void removeHook(const std::string& hook);

	// remove Action.
	void removeAction(const std::string& hook, const string& tag);

	// singleton.
	static shared_ptr<apHookActionManager> getInstance() {

		if(_sp == nullptr) {
			_sp = make_shared<apHookActionManager>();
		}
		return _sp;
	}

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
