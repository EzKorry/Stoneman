#include "FollowNode.h"
#include "IngameScene.h"



bool FollowNode::initWithYardstick(cocos2d::Node * node)
{
	if (!Node::init()) return false;
	if (!node) return false;
	_yardstick = node;

	auto localUpdater = IngameScene::getInstance()->getLocalUpdater();
	localUpdater->addFunc(P_FOLLOWNODE, [this, node](float delta) {
		this->setPosition(-_yardstick->getPosition());
	});
	return true;
}

FollowNode::FollowNode()
{
}


FollowNode::~FollowNode()
{
}

void FollowNode::setPosition(float x, float y)
{
	Node::setPosition(x, y);
}

void FollowNode::setPosition(const Vec2 & position)
{
	Node::setPosition(position);
}
