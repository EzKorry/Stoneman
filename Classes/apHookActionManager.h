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
#include <apDetachManager.h>
#include "function_traits.h"
#include <algorithm>

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
//************ how to use **************
//	auto manager = apHookActionManager::getInstance();
//	addAction("oh_yeah", [](Power power) {
//		power.ohyeah();
//	});
//	Power p;
//	RunHook("oh_yeah", p);
// ***************************************

// action's parameter types can't be a reference or const. (for my weak c++..)
using apActionDeleter = function<void(cocos2d::Node*)>;
using apActionTargetMap = unordered_map<cocos2d::Node*, vector<tuple<string, string>>>;
template <class Func>
using apActionList = unordered_map<string, unordered_map<string, Func>>;
class apHookActionManager : public apDetachInterface {
private:
	

	template <class Func>
	class Actions {
	public:
		static apActionList<Func> list;

		// target, hook, tag.
		static apActionTargetMap targetMap;
		static void deleter(cocos2d::Node* node);
		using type = Func;

	};

	
	
public:

	

	virtual void detach(cocos2d::Node* node) override;


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

	// add function to hook.
	// if the target on exit, action will be removed too.
	// tag is the action's name.
	// we can run hook after a while.
	//	using TFunc = std::function<void(Args...)>;
	template<class TString1, class TString2, class TFunc>
	shared_ptr<apHookActionManager> addAction(TString1&& hook, TString2&& tag, cocos2d::Node* target, TFunc&& action) {
	

		// if target is null, exit!!
		CCASSERT(target, "aphookactionmanager add Action target must not be null");

		using TActions = Actions<
			typename function_traits<
				typename std::decay<TFunc>::type
			>::function_type_decayed
		>;
		auto& list = TActions::list;
		auto& deleter = TActions::deleter;
		auto& targetVector = TActions::targetMap[target];
		//addHook(std::forward<TString1>(hook));

		// if tag already exists, no execution.
		list[std::forward<TString1>(hook)].emplace(std::forward<TString2>(tag), std::forward<TFunc>(action));
		
		tuple<string, string> hook_key_pair = make_tuple(
			std::forward<TString1>(hook),
			std::forward<TString2>(tag)
		);

		// if not found,
		if (std::find(targetVector.begin(), targetVector.end(), hook_key_pair) == targetVector.end()) {
			targetVector.emplace_back(hook_key_pair);
		}
		
		// if the node on exit, HookActionManager will call this.
		if (std::find(_deleter.begin(), _deleter.end(), &deleter) == _deleter.end()) {
			_deleter.emplace_back(&deleter);
		}

		// add detach manager.
		auto detachManager = apDetachManager::getInstance();
		detachManager->addNode(target, apDetachManagerType::HookActionManager);
		
		
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
		auto& list = ActionType::list;
		if (list.find(forward<TString>(hook)) != list.end()) {

			// run each function.
			// seperate from REAL list because of deleting action while that action running.
			auto actionToRun = list[forward<TString>(hook)];
			for (auto& item : actionToRun) {
				item.second();
			}
		}
		return _sp;
	}

	// run hook with type assumption.
	template<class TString, class... Args>
	shared_ptr<apHookActionManager> runHook(TString&& hook, Args&&... args) {
		using ActionType = Actions<std::function<void(typename std::decay<Args>::type...)>>;
		auto& list = ActionType::list;
		if (list.find(forward<TString>(hook)) != list.end()) {
			for (auto& item : list[forward<TString>(hook)]) {
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

	template<class Func>
	static apActionList<Func>& getActionList() {
		return Actions<Func>::list;
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
	std::vector<void (*)(cocos2d::Node*)> _deleter;

	
};

/*template<class Func>
apActionDeleter apHookActionManager::Actions<Func>::deleter = [](cocos2d::Node* node) {
	// if found,
	
};*/

template <class Func>
apActionList<Func> apHookActionManager::Actions<Func>::list = apActionList<Func>();

template <class Func>
apActionTargetMap apHookActionManager::Actions<Func>::targetMap =
apActionTargetMap();

template<class Func>
inline void apHookActionManager::Actions<Func>::deleter(cocos2d::Node * node)
{
	if (targetMap.find(node) != targetMap.end()) {
		for (auto& item : targetMap[node]) {
			auto& hookToDelete = std::get<0>(item);
			auto& tagToDelete = std::get<1>(item);
			//	auto& list = apHookActionManager::Actions<Func>::list;
			auto& l = getActionList<Func>();
			type t;
			if (l.find(hookToDelete) != l.end()) {
				l[hookToDelete].erase(tagToDelete);
			}


		}
	}
}

} /* namespace arphomod */

#endif /* APHOOKACTIONMANAGER_H_ */
