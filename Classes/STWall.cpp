#include "STWall.h"
#include "STMacros.h"


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

void STWall::setPosition(const Vec2 & position)
{
	setPosition(b2Vec2(position.x / SCALE_RATIO, position.y / SCALE_RATIO));
}

void STWall::setPosition(const b2Vec2 & position)
{
	_body->SetTransform(position, _body->GetAngle());
}

Vec2 STWall::getPosition()
{
	auto bp = getb2Position();
	return Vec2(bp.x * SCALE_RATIO, bp.y * SCALE_RATIO);
}

b2Vec2 STWall::getb2Position()
{
	return _body->GetPosition();
}

/*void STWall::runAction(WallButtonActionType type, void * object)
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
}*/

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
		//body->SetUserData(ret);
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