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
#include "function_traits.h"

namespace arphomod {
using namespace std;
using apActionContainer = unordered_map<string, function<void()>>;
using apHookActionContainer = unordered_map<string,apActionContainer>;

class apActionFunc {
public:
	

	template <class Func>
	static shared_ptr<apActionFunc> makeAction(Func&& func) {
		
		Action<Func>::func = func;
	}


	shared_ptr<apActionFunc> next();
	
	template <class... Args>
	void run(Args&&... args) {
		func(std::forward<Args>(args)...);
	}
	void func(int a);
	apActionFunc();
	~apActionFunc();
private:
};
template<class TFunc>
class apActionFuncImpl : public apActionFunc {

};

class apHookActionManager {
private:
	template <class Func>
	class Actions {
	public:
		static unordered_map<string, unordered_map<string, Func>> list;
		using type = Func;

	};

	
public:

	




	apHookActionManager();

	// add event hook.
	/*
	template<class TString>
	shared_ptr<apHookActionManager> addHook(TString&& hook) {

		// if hook not found,
		if(_actions.find(hook) == _actions.end()) {
			_actions.emplace(std::forward<TString>(hook), apActionContainer{});
		}
		return _sp;
	}

	// add function to hook.
	template<class TString1, class TString2, class TFunc>
	shared_ptr<apHookActionManager> addAction(TString1&& hook, TString2&& tag, TFunc&& action) {

		addHook(std::forward<TString1>(hook));

		// if tag not found, ignore.
		_actions[std::forward<TString1>(hook)].emplace(std::forward<TString2>(tag), std::forward<TFunc>(action));
		return _sp;

	}*/

	//add function to hook.
	template<class TString1, class TString2, class TFunc>
	shared_ptr<apHookActionManager> addAction(TString1&& hook, TString2&& tag, TFunc&& action) {
	//	using TFunc = std::function<void(Args...)>;

		auto& list = Actions<
			typename function_traits<
				typename std::decay<TFunc>::type
			>::function_type
		>::list;
		//addHook(std::forward<TString1>(hook));

		// if tag already exists, no execution.
		list[std::forward<TString1>(hook)].emplace(std::forward<TString2>(tag), std::forward<TFunc>(action));
		return _sp;

	}

	
	/*
	template<class TString>
	shared_ptr<apHookActionManager> runHook(TString&& hook) {
		// if hook found,
		using ActionType = Actions<std::function<void()>>;
		if (::list.find(forward<TString>(hook)) != _actions.end()) {

			// run each function.
			for (auto& item : _actions[hook]) {
				item.second();
			}
		}

		runHook("abc", 1, 4);
		return _sp;
	}*/
	// run hook. then all that function will be invoked.
	template<class TString>
	shared_ptr<apHookActionManager> runHook(TString&& hook) {
		// if hook found,
		using ActionType = Actions<std::function<void()>>;
		if (ActionType::list.find(forward<TString>(hook)) != ActionType::list.end()) {

			// run each function.
			for (auto& item : ActionType::list[forward<TString>(hook)]) {
				item.second();
			}
		}
		return _sp;
	}

	// run hook with type assumption.
	template<class TString, class... Args>
	shared_ptr<apHookActionManager> runHook(TString&& hook, Args&&... args) {
		using ActionType = Actions<std::function<void(Args...)>>;
		if (ActionType::list.find(forward<TString>(hook)) != ActionType::list.end()) {
			for (auto& item : ActionType::list[forward<TString>(hook)]) {
				item.second(std::forward<Args>(args)...);
			}
		}
		return _sp;
	}

	// remove hook.
	//shared_ptr<apHookActionManager> removeHook(const std::string& hook);
	template<class TString>
	shared_ptr<apHookActionManager> removeHook(TString&& hook) {
		return removeHook<void()>(std::forward<TString>(hook));
	}

	// TFunctionType=void()
	template<class TFunctionType, class TString>
	shared_ptr<apHookActionManager> removeHook(TString&& hook) {

		// if hook found,
		using ActionType = Actions<std::function<TFunctionType>>;
		if (ActionType::list.find(std::forward<TString>(hook)) != ActionType::list.end()) {

			ActionType::list.erase(std::forward<TString>(hook));
		}
		return _sp;
	}

	// remove Action.
	//shared_ptr<apHookActionManager> removeAction(const std::string& hook, const string& tag);
	template<class TString1, class TString2>
	shared_ptr<apHookActionManager> removeAction(TString1&& hook, TString2&& tag) {
		return removeAction<void()>(std::forward<TString1>(hook), std::forward<TString2>(tag));
	}


	template<class TFunctionType, class TString1, class TString2>
	shared_ptr<apHookActionManager> removeAction(TString1&& hook, TString2&& tag) {

		// if hook found,
		auto& list = Actions<std::function<TFunctionType>>::list;
		auto actionIt = list.find(std::forward<TString1>(hook));
		if (actionIt != list.end()) {
			auto& map = (*actionIt).second;
			auto funcIt = map.find(std::forward<TString2>(tag));
			if (funcIt != map.end()) {
				map.erase(funcIt);
			}
		}
		return _sp;
	}

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
	//apHookActionContainer _actions;

	static shared_ptr<apHookActionManager> _sp;

	
};

template <class Func>
unordered_map<string, unordered_map<string, Func>> apHookActionManager::Actions<Func>::list = unordered_map<string, unordered_map<string, Func>>();

} /* namespace arphomod */

#endif /* APHOOKACTIONMANAGER_H_ */
