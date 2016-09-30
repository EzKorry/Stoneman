#include "STTextBoxNode.h"
#include "STLabel.h"
#include "IngameScene.h"
#include "Box2D/Box2D.h"

Node* STTextBoxNode::createTextBox(const string & text, const Vec2 & vec)
{
	auto spc = SpriteFrameCache::getInstance();
	auto nod = Node::create();
	nod->setPosition(vec);
	this->addChild(nod);

	TTFConfig conf;
	conf.fontSize = 14;
	conf.fontFilePath = IngameScene::getInstance()->getSettings()
		["font_paths"]["nanum_square_regular"].GetString();
	auto textLabel = STLabel::createWithTTF(conf, text, TextHAlignment::CENTER, 100);
	textLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
	textLabel->setLocalZOrder(3);

	nod->addChild(textLabel);
	
	auto size = textLabel->getSizeCalcResolution();
	auto box = Sprite::createWithSpriteFrameName("square.png");
	box->setScaleX((size.width + 30) / box->getContentSize().width);
	box->setScaleY((size.height + 30) / box->getContentSize().height);
	box->setLocalZOrder(2);
	box->setCascadeOpacityEnabled(true);
	box->setCascadeColorEnabled(true);
	box->setOpacity(125);
	box->setColor(Color3B(0, 0, 0));
	nod->addChild(box);


	
	auto wait = DelayTime::create(0.2f * textLabel->getStringLength());
	auto remove = CallFunc::create([nod]() {
		nod->removeFromParent();
	});
	auto sequence = Sequence::create(wait, remove, nullptr);
	IngameScene::getInstance()->runLocalAction(nod, sequence);

	auto detachManager = apDetachManager::getInstance();
	detachManager->addNode(nod);

	return nod;
	
	
	//auto box = Sprite::createWithSpriteFrameName()
	//this->addChild()
}

void STTextBoxNode::createFollowingTextBox(const string & text, b2Body * body, const Vec2 & offset)
{
	auto pos = Vec2(body->GetPosition().x * SCALE_RATIO + offset.x, body->GetPosition().y * SCALE_RATIO + offset.y);
	auto nod = createTextBox(text, pos);

	auto scene = IngameScene::getInstance();
	
	scene->scheduleLocally([body, nod, offset](float detla) {
		auto pos = Vec2(body->GetPosition().x * SCALE_RATIO + offset.x, body->GetPosition().y * SCALE_RATIO + offset.y);
		nod->setPosition(pos);
	}, nod, "follow");

	/*
	auto sc = scene->getLocalScheduler();
	sc->schedule([body, nod, offset](float detla) {
		auto pos = Vec2(body->GetPosition().x * SCALE_RATIO + offset.x, body->GetPosition().y * SCALE_RATIO + offset.y);
		nod->setPosition(pos);
	}, nod, 0, false, "follow");
	auto detachManager = apDetachManager::getInstance();
	detachManager->addNode(nod);*/
}

STTextBoxNode::STTextBoxNode()
{
}

STTextBoxNode::~STTextBoxNode()
{
}
