#include "STWall.h"



STWall::STWall(const std::string& name,
	WallType wallType,
	const Rect& rect,
	/*const std::vector<b2Fixture*>& fixtures,*/
	b2Body* body) :

	_name(name),
	_wallType(wallType),
	_rect(rect),
	//_fixtures(fixtures),
	_body(body)
{
}


std::string STWall::getName()
{
	return _name;
}

WallType STWall::getWallType()
{
	return _wallType;
}

Rect STWall::getRect()
{
	return _rect;
}
/*
std::vector<b2Fixture*> STWall::getFixtures()
{
	return _fixtures;
}*/

b2Body * STWall::getBody()
{
	return _body;
}

void STWall::runAction(WallButtonActionType type, void * object)
{
	switch (type) {
	case WallButtonActionType::TurnButtonOff:

		break;
	case WallButtonActionType::TurnButtonOn:

		break;
	case WallButtonActionType::TurnToBreakableWall:

		break;
	case WallButtonActionType::TurnToSolidWall:

		break;
	default:

		break;
	}
}

void STWall::setWallType(WallType wallType)
{
	_wallType = wallType;
}

STWall::~STWall()
{
}

STWall * STWall::create(
	const std::string& name,
	WallType wallType,
	const Rect& rect,
	//const std::vector<b2Fixture*>& fixtures,
	b2Body* body)
{
	STWall * ret = new (std::nothrow) STWall(name, wallType, rect, /*fixtures,*/ body);
	if (ret && ret->init())
	{

	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}
bool STWall::init() {

	return true;
}