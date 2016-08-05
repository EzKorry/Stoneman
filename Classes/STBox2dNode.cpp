#include "STBox2dNode.h"
#include "STMacros.h"
#include "IngameScene.h"



STBox2dNode::STBox2dNode()
{
}


STBox2dNode::~STBox2dNode()
{
}

STBox2dNode * STBox2dNode::createWithBox2dBody(b2Body* body)
{
	STBox2dNode * ret = new (std::nothrow) STBox2dNode();
	if (ret && ret->initWithBox2dBody(body))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

void STBox2dNode::setBodyPosition(const Vec2 & position)
{
	_body->SetTransform(b2Vec2(position.x / SCALE_RATIO, position.y / SCALE_RATIO), _body->GetAngle());
}

void STBox2dNode::setBodyPositionBlock(const Vec2 & position)
{
	setBodyPosition(Vec2(position.x * IngameScene::OneBlockPx, position.y * IngameScene::OneBlockPx));
}

bool STBox2dNode::initWithBox2dBody(b2Body* body) {
	if (!body) return false;
	_body = body;
	//this->setAnchorPoint(Vec2(0.5f, 0.5f));
	return true;
}