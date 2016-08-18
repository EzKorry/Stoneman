/*
 * TouchManager.cpp
 *
 *  Created on: 2014. 11. 19.
 *      Author: EzKorry
 */

#include <memory>
#include <functional>
#include <algorithm>
#include <future>
#include "apTouchManager.h"
#include "cocos2d.h"


namespace arphomod {

std::shared_ptr<APTouchManager> APTouchManager::_sp = nullptr;
std::shared_ptr<APTouchManager> APTouchManager::getInstance() {

	if(_sp == nullptr) {

		_sp = std::make_shared<APTouchManager>();
		_sp->initWithNodeToAttatch();
	}
	return _sp;
}

void APTouchManager::initWithNodeToAttatch() {

	for(int i=0;i<6;i++) {
		_touchToBeganNode.emplace(i, nullptr);
		_touchToNowNode.emplace(i, nullptr);
	}
	attachCallback();
}

void APTouchManager::attachCallback(){
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->onTouchBegan = std::bind(&APTouchManager::onTouchBegan, this, std::placeholders::_1, std::placeholders::_2);
	listener->onTouchMoved = std::bind(&APTouchManager::onTouchMoved, this, std::placeholders::_1, std::placeholders::_2);
	listener->onTouchEnded = std::bind(&APTouchManager::onTouchEnded, this, std::placeholders::_1, std::placeholders::_2);
	listener->onTouchCancelled = std::bind(&APTouchManager::onTouchCancelled, this, std::placeholders::_1, std::placeholders::_2);
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 100);
}


void APTouchManager::detach(cocos2d::Node * node)
{
	cocos2d::log("aptouchmanager detach called!");
	removeNode(node);
}

void APTouchManager::registerNode(cocos2d::Node* node, APTouchChecker checker) {

	if (_d.find(node) != _d.end()) {
		_d[node].checker = checker;

	} else {
		// node retain.
		node->retain();
		_d.emplace(node, APTouchData(checker));
	}

	auto detachManager = apDetachManager::getInstance();
	detachManager->addNode(node, apDetachManagerType::TouchManager);
}


void APTouchManager::addHook(cocos2d::Node * node, APTouchType timing, const std::string & hook)
{
	// lazy initializing.
	if (_amp == nullptr) {
		_amp = apHookActionManager::getInstance();
	}
	//_amp->addHook(hook);
	_d[node].hook.emplace(timing, hook);
}

// delete behavior
void APTouchManager::delBehavior(const std::string& hook, const std::string& behaviorTag) {
	if(_amp == nullptr) {
		_amp = apHookActionManager::getInstance();
	}
	_amp->removeAction(hook, behaviorTag);
}


void APTouchManager::setChecker(cocos2d::Node* node, APTouchChecker checker) {
	if (_d.find(node) != _d.end()) {
		_d[node].checker = checker;
	}
}

void APTouchManager::setEnabledManager(bool enabled) {
	_enabled = enabled;
}

cocos2d::Touch* APTouchManager::getTouch() {
	return _nowTouch;
}

void APTouchManager::setEnabledNode(cocos2d::Node* node, bool enabled) {
	if (_d.find(node) != _d.end()) {
		_d[node].enabled = enabled;
	}
}

void APTouchManager::setEnabledAll(bool enabled) {
	for(auto& item : _d) {
		item.second.enabled = enabled;
	}
}

void APTouchManager::setMaxTouch(cocos2d::Node* node, int max) {
	if (_d.find(node) != _d.end()) {
		_d[node].maxTouch = max;
	}
}

void APTouchManager::setOrder(cocos2d::Node* node, int order) {
	if (_d.find(node) != _d.end()) {
		_d[node].order = order;
	}


}


void APTouchManager::clear() {

	std::list<cocos2d::Node*> nodes;
	for(auto& item:_d) {
		nodes.emplace_back(item.first);
	}
	for(auto& item:nodes) {
		removeNode(item);
	}
	_d.clear();

}

void APTouchManager::removeNode(cocos2d::Node* node) {
	if(_amp == nullptr) {
		_amp = apHookActionManager::getInstance();
	}

	if (_d.find(node) != _d.end()) {

		for(auto& item:_d[node].hook) {
			_amp->removeHook(item.second);
		}
		auto result = _d.erase(node);
		//cocos2d::log("%d memer deleted! from touch manager _d map.",result);


		node->release();
		//cocos2d::log("node released!from touch manager --> %d",node);
	}

	//debug_d();
}

void APTouchManager::cancelAllTouch() {
	for (int i = 0; i < 6; i++) {
		_touchToBeganNode[i] = nullptr;
		_touchToNowNode[i] = nullptr;
	}
}


APTouchChecker APTouchManager::createDefaultChecker(cocos2d::Node* node) {
	auto size = node->getContentSize();

	return [size,node](cocos2d::Touch* touch)->bool {
		auto position = node->getPosition();
		auto anchor = node->getAnchorPoint();
		auto parent = node->getParent();
		if (parent != nullptr) {
			position = parent->convertToWorldSpace(position);
		}
		auto rect = cocos2d::Rect(position.x - size.width * anchor.x,
					position.y - size.height * anchor.y, size.width, size.height);

		////cocos2d::log("name: %s, %f %f %f %f",node->getName().c_str(), rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);

		return rect.containsPoint(touch->getLocation());

	};
}

cocos2d::Rect APTouchManager::createDefaultRect(cocos2d::Node* node) {
	auto size = node->getContentSize();
	auto position = node->getPosition();
	auto anchor = node->getAnchorPoint();
	auto parent = node->getParent();
	if (parent != nullptr) {
		position = parent->convertToWorldSpace(position);
	}

	auto rect = cocos2d::Rect(position.x - size.width * anchor.x,
			position.y - size.height * anchor.y, size.width, size.height);
	//cocos2d::log("%f %f %f %f",position.x - size.width * anchor.x,
			//position.y - size.height * anchor.y, size.width, size.height);
	return rect;

}

APTouchChecker APTouchManager::createCheckerWithRect(const cocos2d::Rect& rect) {

	return [rect](cocos2d::Touch* touch)->bool{
		return rect.containsPoint(touch->getLocation());

	};
}

APTouchChecker APTouchManager::createCheckerWithRect(cocos2d::Node* node, const cocos2d::Rect& rect) {

	return [rect, node](cocos2d::Touch* touch)->bool {
		auto position = node->getPosition();
		auto parent = node->getParent();
		if (parent != nullptr) {
			position = parent->convertToWorldSpace(position);
		}

		auto r = cocos2d::Rect(rect);
		r.origin += position;
		return r.containsPoint(touch->getLocation());
	};
}

bool APTouchManager::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event){

	//if touch count is over 6
	//if(touch->getID() > 5) return true;
/*
	if (_touchIDMap.find(touch->getID()) == _touchIDMap.end()) {
		_touchIDMap.insert(decltype(_touchIDMap)::value_type(touch->getID, nullptr));
	} else {
		//cocos2d::log("weird touchStarted");
	}
*/
	if(_amp == nullptr) {
		_amp = apHookActionManager::getInstance();
	}

	auto nowNode = getTouchedNode(touch);

	_touchToBeganNode[touch->getID()] = nowNode;
	_touchToNowNode[touch->getID()] = nowNode;


	runHook(nowNode, APTouchType::Began, touch);/*
	if (nowNode && !isBehaviorNullptr(nowNode, APTouchType::Began)) {
		_behaviorMap[APTouchType::Began][nowNode](touch);
	}*/

	return true;
/*
	for(const auto& item :_checkerMap) {
		if(item.second(touch) && _globalZOrderVec.at(item.first) > biggest) {
			node = item.first;
			biggest = _globalZOrderVec.at(item.first);
		}
	}
	if (node) {
		_behaviorMap.at(node)();
	}
*/
}

void APTouchManager::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event){

	debug_d();
	auto id = touch->getID();

	if(id > 5) return;

	if(_amp == nullptr) {
		_amp = apHookActionManager::getInstance();
	}
	// if not began
	//if (_touchToBeganNode[id] == nullptr || id > 5)

	// they can be nullptr.
	auto nowNode = getTouchedNode(touch);
	auto beforeNode = _touchToNowNode[id];
	_touchToNowNode[id] = nowNode;

	//cocos2d::log("yo");

	auto originNode = _touchToBeganNode[id];
	// outside
	//auto& originNodeHook = _d[originNode].hook;
	//auto& nowNodeHook = _d[nowNode].hook;

	// originNode : node when began that touch.
	// nowNode: now touching node.
	// beforeNode: last touched node.

	// go in or go out. now touching node differs with the last touched one.

	////cocos2d::log("nowNode:%d, beforeNode:%d, originNode:%d", nowNode, beforeNode, originNode);

	if(beforeNode != nowNode) {
		//cocos2d::log("yo2");
		// drag to origin node
		if(nowNode == originNode) {
			runHook(nowNode, APTouchType::MovedInner, touch);
			runHook(beforeNode, APTouchType::MovedOuterIgnoreBegan, touch);
		}

		// if dragged not to origin node and the last node was origin Node
		else if(beforeNode == originNode) {
			runHook(nowNode, APTouchType::MovedInnerIgnoreBegan, touch);
			runHook(beforeNode, APTouchType::MovedOuter, touch);
		}

		// all different!
		else {
			runHook(nowNode, APTouchType::MovedInnerIgnoreBegan, touch);
			runHook(beforeNode, APTouchType::MovedOuterIgnoreBegan, touch);
		}
	}

	// before node and now node are same.
	else {
		//cocos2d::log("yo3");
		// if all same.
		if(nowNode == originNode) {
			runHook(nowNode, APTouchType::MovedInside, touch);
		}
		// if only origin node differs.
		else {
			runHook(nowNode, APTouchType::MovedInsideIgnoreBegan, touch);
			runHook(originNode, APTouchType::MovedOutside, touch);
		}

	}
	//cocos2d::log("yo4");




/*




	if (beforeNode != originNode && nowNode != originNode
			&& originNodeHook.find(APTouchType::MovedOutside) != originNodeHook.end()) {

		if(nowNode )
		_amp->runHook(originNodeHook[APTouchType::MovedOutside]);
	}
	// inside
	else if (beforeNode == originNode && nowNode == originNode
			&& originNodeHook.find(APTouchType::MovedInside) != originNodeHook.end()) {
		_amp->runHook(originNodeHook[APTouchType::MovedInside]);
	}

	// go to out
	else if (beforeNode == originNode && nowNode != originNode
			&& originNodeHook.find(APTouchType::MovedOuter) != originNodeHook.end()) {
		_amp->runHook(originNodeHook[APTouchType::MovedOuter]);

	}

	// inner
	else if (beforeNode != originNode && nowNode == originNode
			&& originNodeHook.find(APTouchType::MovedInner) != originNodeHook.end()) {
		_amp->runHook(originNodeHook[APTouchType::MovedInner]);
	}
*/

}

inline void APTouchManager::runHook(cocos2d::Node* node, APTouchType touchType, cocos2d::Touch* nowTouch) {
	_nowTouch = nowTouch;
	if(node && _d.find(node) != _d.end()) {
		auto& hook = _d[node].hook;
		if(hook.find(touchType) != hook.end())
			_amp->runHook(hook[touchType]);

	}
	_nowTouch = nullptr;
}

void APTouchManager::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event){
	touchEnded(touch, false);
}
void APTouchManager::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event){
	touchEnded(touch, true);

}
void APTouchManager::touchEnded(cocos2d::Touch* touch, bool isCancelled) {
	auto id = touch->getID();
	// if not began
	if (id > 5)
		return;

	// they can be nullptr.
	auto originNode = _touchToBeganNode[id];
	auto nowNode = getTouchedNode(touch);

	if(isCancelled) {
		runHook(originNode, APTouchType::Cancelled, touch);

	}
	// if presently touching node differs with origin node
	else if (nowNode != originNode) {
		runHook(originNode, APTouchType::EndedOut, touch);
		runHook(nowNode, APTouchType::EndedInIgnoreBegan, touch);

	} else {
		runHook(originNode, APTouchType::EndedIn, touch);

	}

	_touchToBeganNode[id] = nullptr;
	_touchToNowNode[id] = nullptr;


	/*
	if (_touchIDMap.find(touch->getID()) != _touchIDMap.end()) {
		_touchIDMap.erase(touch->getID());
	} else {
		//cocos2d::log("weird touchEnded");
	}*/
}

/*
void APTouchManager::onEvent(cocos2d::Touch* touch, cocos2d::Event* event, APTouchType timing) {
	std::vector<cocos2d::Node*> TouchedNodes;

	for (const auto& item : _checkerMap) {
		if (item.second(touch)) {
			TouchedNodes.push_back(item.first);
		}
	}

	if (TouchedNodes.empty()) {
		return;
	} else {

		std::sort(TouchedNodes.begin(), TouchedNodes.end(),
				[](const cocos2d::Node* left, const cocos2d::Node* right)->bool {
					return left->getGlobalZOrder() > right->getGlobalZOrder();
				});
		if (_behaviorMap[timing][TouchedNodes[0]] != nullptr) {
			_behaviorMap[timing][TouchedNodes[0]](touch);
		}
	}

}*/

cocos2d::Node* APTouchManager::getTouchedNode(cocos2d::Touch* touch) {


	std::vector<cocos2d::Node*> TouchedNodes;
	std::vector<cocos2d::Node*> NodesToBeRemoved;
	//cocos2d::log("a");
	for (const auto& item : _d) {

		auto node = item.first;
		auto& data = item.second;
		//cocos2d::log("b");
		// check if item is not available. then push to remove vector
		// 널포인트는 아닌데, dangled일 경우 에러발생!!

		//cocos2d::log("checking node is --> %d", node);
		if (node && !node->isRunning() && node->getParent() == nullptr){
			NodesToBeRemoved.emplace_back(node);
			//cocos2d::log("nodes to be removed invoke!!");
		}

		// checker true and enabled true and max Touch is irrelevant
		else if (node && node->isRunning() && data.checker(touch) && data.enabled && touch->getID() < data.maxTouch)
			TouchedNodes.emplace_back(node);
		//cocos2d::log("SUCCESS TO DIVIDE Role!");

	}

	// node remove
	for (const auto& item : NodesToBeRemoved) {
		removeNode(item);
	}
	//cocos2d::log("ad");
	// nothing touched.
	if (TouchedNodes.empty()) {
		return nullptr;
	}

	// something touched
	else {
		//cocos2d::log("e");
		// sort
		std::sort(TouchedNodes.begin(), TouchedNodes.end(),
				[this](cocos2d::Node* left, cocos2d::Node* right)->bool {
					return _d.at(left).order > _d.at(right).order;
				});
		return TouchedNodes[0];

	}


}


cocos2d::Vec2 APTouchManager::getAbsolutePosition(cocos2d::Node* node) {
	auto position = node->getPosition();

	auto parent = node->getParent();
	if (parent != nullptr) {
		position = parent->convertToWorldSpace(position);
	}

	return position;

}
void APTouchManager::touchStarted(cocos2d::Touch* touch) {

}

void APTouchManager::debug_d() {/*
	int i = 0;
	for(auto& item : _d) {
		//cocos2d::log("%d: hookCount:%d", i, item.second.hook.size());

		i++;
	}
*/
}


APTouchManager::~APTouchManager() = default;

APTouchManager::APTouchManager() = default;

APTouchManager::APTouchManager(APTouchManager&& rhs) = default;

APTouchManager& APTouchManager::operator=(APTouchManager&& rhs) = default;


}
