#include "STStageEventMaker.h"
#include "IngameScene.h"
#include "DebugBox.h"
#include "STWall.h"

void STStageEventMaker::makeStageEvent(const string & level)
{
	if (level == "1") makeLevel1();
}

STStageEventMaker::STStageEventMaker()
{
}


STStageEventMaker::~STStageEventMaker()
{
}

void STStageEventMaker::makeLevel1()
{	
	auto scene = IngameScene::getInstance();
	auto cl = scene->getContactListener();
	auto charBody = scene->getCharBody();
	cl->setBeginContact(charBody, [scene](b2Contact* contact, b2Fixture* other) {
		auto wall = scene->getWallByBody(other->GetBody());
		scene->getDebugBox()->get() << wall->getName() << arphomod::DebugBox::push;
	});
}
