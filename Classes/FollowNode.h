#ifndef _FOLLOW_NODE_H_
#define _FOLLOW_NODE_H_

#include <cocos2d.h>
using namespace cocos2d;
class FollowNode : public cocos2d::Node
{
public:
	static FollowNode* createWithYardstick(cocos2d::Node* node)
	{
		FollowNode *pRet = new (std::nothrow) FollowNode();
		if (pRet && pRet->initWithYardstick(node))
		{
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = NULL;
			return NULL;
		}
	}
	bool initWithYardstick(cocos2d::Node* node);
	FollowNode();
	virtual ~FollowNode();
private:
	virtual void setPosition(float x, float y) override;
	virtual void setPosition(const Vec2& position) override;
	cocos2d::Node* _yardstick;
};

#endif