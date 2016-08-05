#ifndef _STBOX2DNODE_H_
#define _STBOX2DNODE_H_

#include <cocos2d.h>
#include <Box2D\Box2D.h>
using namespace cocos2d;

class STBox2dNode :
	public cocos2d::Node
{
public:

	static STBox2dNode* createWithBox2dBody(b2Body* body);

	virtual void setBodyPosition(const Vec2 &position);
	
	virtual void setBodyPositionBlock(const Vec2 &position);

	virtual bool initWithBox2dBody(b2Body* body);



	STBox2dNode();
	virtual ~STBox2dNode();
private:

	b2Body* _body;
};

#endif