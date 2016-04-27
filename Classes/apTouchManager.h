/*
 * TouchManager.h
 *
 *  Created on: 2014. 11. 19.
 *      Author: EzKorry
 */

#ifndef APTOUCHMANAGER_H_
#define APTOUCHMANAGER_H_

#include <sstream>
#include <string>
#include "cocos2d.h"
#include "apHookActionManager.h"


/* how to use:
 * 1. #include "apTouchManager.h"
 * 2. auto manager = APTouchManager::create();
 * 3. manager->registerNode(~~~);
 * 4. manager->setBehavior(~~~);
 */

namespace arphomod {

/**
 * - Began: TouchBegan when each Checker true.
 * - MovedInner: TouchMoved out to in. required Began.
 * - MovedInside: TouchMoved in to in. required Began.
 * - MovedOuter: TouchMoved in to out. required Began.
 * - MovedOutside: TouchMoved out to out. required Began.
 * - MovedInnerIgnoreBegan: TouchMoved out to in. not required Began.
 * - MovedInsideIgnoreBegan: TouchMoved in to in. not required Began.
 * - MovedOuterIgnoreBegan: TouchMoved in to out. not required Began.
 * - EndedIn: TouchEnded inside. required Began.
 * - EndedInIgnoreBegan: TouchEnded inside. not required Began.
 * - EndedOut: TouchEnded outside. required Began.
 * - Cancelled: TouchCancelled. required Began.
 */


enum class APTouchType{
	Began,
	MovedInner,
	MovedInside,
	MovedOuter,
	MovedOutside,
	MovedInnerIgnoreBegan,
	MovedInsideIgnoreBegan,
	MovedOuterIgnoreBegan,
	EndedIn,
	EndedInIgnoreBegan,
	EndedOut,
	Cancelled,

};

enum class APAttribute{
	Checker,

};


using APTouchChecker = std::function<bool(cocos2d::Touch*)>;
using APTouchBehavior = std::function<void()>;
using APTouchBehaviorMap = std::unordered_map<cocos2d::Node*, APTouchBehavior>;

struct APTouchData {
	APTouchData():checker(
			[](cocos2d::Touch*)->bool {
		return false;
	}){}

APTouchData(APTouchChecker checker) : checker(checker) {

	}
	std::map<APTouchType, std::string> hook;
	APTouchChecker checker;
	bool enabled = true;
	bool hasExclusiveTouch = false;
	int maxTouch = 6;
	int order = 0;
};

class APTouchManager{
public:



	// initialize Nodes
	// default order: 0
	// default max Touch: 6
	void registerNode(cocos2d::Node* node, APTouchChecker checker);

	void initWithNodeToAttatch();

	// singleton
	static std::shared_ptr<APTouchManager> getInstance();

	// set behavior with specified timing
	// you can do with same behavior tag and different behavior function.
	/*
	 * @param
	 * node: you will add behavior to node that has unique checker.
	 * timing: behavior executes at this timing.
	 * behavior: void().
	 * hook: apTouchManager works with apHookActionManager. this hook is used that.
	 * behaviorTag: this is used to actionTag in apHookActionManager. default="_from_tm"
	 */
	template<typename TFunc>
	void addBehavior(cocos2d::Node* node,APTouchType timing, TFunc&& behavior, const std::string& hook, const std::string& behaviorTag = "_from_tm") {
		// lazy initializing.
		if(_amp == nullptr) {
			_amp = apHookActionManager::getInstance();
		}
		_amp->addAction(hook, behaviorTag, std::forward<TFunc>(behavior));
		_d[node].hook.emplace(timing, hook);
		cocos2d::log("addBehavior Executed!! string:%s, behaviorTag:%s", hook.c_str(), behaviorTag.c_str());
	}

	// delete behavior
	void delBehavior(const std::string& hook, const std::string& behaviorTag);

	// set checker that says when it is true if touch point is in custom area.
	void setChecker(cocos2d::Node* node, APTouchChecker checker);

	// set that this manager is enabled.
	void setEnabledManager(bool enabled);

	// get now touch.
	cocos2d::Touch* getTouch();

	// true if it could have only one touch.
	// false if it could have multiple touch.
	void setHasExclusiveTouch(bool excluded);

	// set that something node is enabled.
	void setEnabledNode(cocos2d::Node* node, bool enabled);

	// set that all node is enabled.
	void setEnabledAll(bool enabled);

	// set something node's max touch. default: 6 (touch id:0~5)
	void setMaxTouch(cocos2d::Node* node, int max);

	// set something node's order. higher number indicates higher priority.
	void setOrder(cocos2d::Node* node, int order);

	// reset touch manager.
	void clear();

	// unregister node.
	void removeNode(cocos2d::Node* node);

	// if this execute when something touch is going, touch immediately end.
	// no callback(ended, cancelled) executes.
	void cancelAllTouch();

	void debug_d();

	// create node's default checker which uses getSize(), containsPoint()
	// node must have anchor point (0.5,0.5)
	static APTouchChecker createDefaultChecker(cocos2d::Node* node);

	static cocos2d::Rect createDefaultRect(cocos2d::Node* node);

	// create checker with rect. it is absolute.
	static APTouchChecker createCheckerWithRect(const cocos2d::Rect& rect);

	// create checke with rect following node. it is relative rect.
	// if with Rect(-20, -30, 100, 100), node positioning (200,200), then you can touch
	// (180, 170, 100, 100) rect at that time.
	static APTouchChecker createCheckerWithRect(cocos2d::Node* node, const cocos2d::Rect& rect);

	virtual ~APTouchManager();
	APTouchManager();
	APTouchManager(APTouchManager&& rhs);
	APTouchManager& operator=(APTouchManager&& rhs);

private:
	// initialize Callbacks
	void attachCallback();

	// callbacks
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
	inline bool isBehaviorNullptr(cocos2d::Node* node, APTouchType type);
	inline cocos2d::Vec2 getAbsolutePosition(cocos2d::Node* node);
	inline void runHook(cocos2d::Node*, APTouchType touchType, cocos2d::Touch* nowTouch);

	void touchStarted(cocos2d::Touch* touch);
	void touchEnded(cocos2d::Touch* touch, bool isCancelled = false);

	cocos2d::Node* getTouchedNode(cocos2d::Touch* touch);

	//void onEvent(cocos2d::Touch* touch, cocos2d::Event* event, APTouchType timing);



	std::unordered_map<cocos2d::Node*, APTouchData> _d;
	/*std::unordered_map<cocos2d::Node*, APTouchChecker> _checkerMap;
	std::unordered_map<cocos2d::Node*, bool> _enabledMap;
	std::unordered_map<cocos2d::Node*, int> _maxTouchMap;
	std::unordered_map<cocos2d::Node*, int> _nodeToOrder;*/
	std::unordered_map<int, cocos2d::Node*> _touchToNowNode;
	std::unordered_map<int, cocos2d::Node*> _touchToBeganNode;
	std::unordered_map<int, cocos2d::Touch*> _touchIdToTouch;

	static std::shared_ptr<APTouchManager> _sp;
	std::shared_ptr<apHookActionManager> _amp;

	bool _enabled{true};
	cocos2d::Touch* _nowTouch{nullptr};



};

template<typename T>
std::string NumberToString(T Number) {
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}


}

#endif /* APTOUCHMANAGER_H_ */
