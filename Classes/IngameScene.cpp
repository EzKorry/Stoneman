/*
 * IngameScene.cpp
 *
 *  Created on: 2015. 10. 15.
 *      Author: 김태훈
 */

#include "IngameScene.h"
#include "GLES-Render.h"
#include "STCamera.h"
#include "apTouchManager.h"
#include "STTextField.h"
#include <string>
#include "SimpleAudioEngine.h"
#include "STLabel.h"
#include "AppDelegate.h"
#include "STWallBuilder.h"
#include "apAsyncTaskManager.h"
#include "DebugBox.h"
#include <sstream>
#include <future>
#include <boost/locale.hpp>
#include <fstream>

USING_NS_CC;

using namespace boost::locale;
using namespace std;
float IngameScene::_timeScale = 1.0f;
std::shared_ptr<b2World> IngameScene::world = nullptr;
const float IngameScene::OneBlockPx = 20.f;
IngameScene* IngameScene::_uniqueScene = nullptr;

bool IngameScene::init() {

	if(!Scene::init()) return false;

	/// REQUIRED!!****
	appInit();
	///***************

	/// Organize Scene. Game Loading, Game Scene turning using apHookActionManager.
	organizeScene();
	///***************

	scheduleUpdate();
	apHookActionManager::getInstance()->runHook("game_start");
	/*
	initializeEffectManager();
	initializePhysics();
	debugVariable();
	gameInterface();
	animationTest();
	*/
	
	
	return true;
}

void IngameScene::appInit()
{
	//set unique scene pointer.
	_uniqueScene = this;

	//prepareThings();
	//go();

	//debugbox test
	auto visibleSize = Director::getInstance()->getVisibleSize();
	_debugBox = DebugBox::create();
	addChild(_debugBox, 999);
	_debugBox->setAnchorPoint(Vec2::ZERO);
	_debugBox->setPosition(Vec2(visibleSize.width, 0.f));
	_debugBox->get() << "abce" << 123 << 1.234 << DebugBox::push;


	auto actionManager = apHookActionManager::getInstance();

	//json Test
	/*json j;
	j.push_back("foo");
	j.push_back(1);
	j.push_back(true);

	stringstream ss;
	// iterate the array
	for (json::iterator it = j.begin(); it != j.end(); ++it) {
	ss << *it << '\n';
	cocos2d::log("%s", ss.str().c_str());
	ss.clear();
	}

	// range-based for
	for (auto& element : j) {
	ss << element << '\n';
	cocos2d::log("%s", ss.str().c_str());
	ss.clear();
	}
	*/
	auto fu = FileUtils::getInstance();
	auto writablePath = fu->getWritablePath();
	_debugBox->get() << writablePath << DebugBox::push
		<< fu->fullPathForFilename("char2.png") << DebugBox::push;
	for (auto& item : fu->getSearchPaths()) {
		_debugBox->get() << item << DebugBox::push;
	}

	// boost localization example.
	generator gen;
	// Specify location of dictionaries
	gen.add_messages_path("./locale");
	gen.add_messages_domain("hello");

	//Same With locale::global(gen("ko_KR.UTF-8"));
	std::locale::global(gen(""));
	

	
	_debugBox->imbue(locale());
	while (0) {
		float speed = 1.5f;
		_debugBox->get() << translate("Hello World") << DebugBox::push
			<< format(translate("You are so fast by speed {1}.")) % speed << DebugBox::push
			<< format(translate("Ganda {1} and so {2} to {3}")) % speed % "ssss" % "ssssdd"
			<< DebugBox::push;
	}

	// custom Scheduler initializing.
	_localScheduler = new (std::nothrow) Scheduler();
	// action manager
	_localActionManager = new (std::nothrow) ActionManager();
	_localScheduler->scheduleUpdate(_localActionManager, Scheduler::PRIORITY_SYSTEM, false);

	// updateCaller initialize.
	_localUpdater = make_shared<UpdateCaller>();
	_globalUpdater = make_shared<UpdateCaller>();
	_localUpdater->initWithScheduler(_localScheduler, "localUpdater");
	_globalUpdater->initWithScheduler(getScheduler(), "globalScheduler");
	boost::coroutines::symmetric_coroutine<void>::call_type updateCallFuncs(
			[this](boost::coroutines::symmetric_coroutine<void>::yield_type& yield) {
	});


	// Json load
	auto js = fu->getStringFromFile("map.json");
	_maps = json::parse(js);


	// initialize member variables.
	_boxWidth = 30.f;
	_boxHeight = 48.f;

	//camera and master field initialize
	_camera = STCamera::create();
	auto touchManager = APTouchManager::getInstance();
	_masterField = Node::create();
	_box2dWorld = Node::create();


	_camera->setField(_masterField);

	this->addChild(_camera, 5, "camera");
	this->addChild(_masterField, 0, "masterField");
	_masterField->addChild(_box2dWorld, 0, "box2dWorld");

	


	// font test
	/*TTFConfig conf("fonts/SpoqaHsRegular.ttf", 24);
	auto label = STLabel::createWithTTF(conf, (format(translate("You are so fast by speed {1}.")) % 4.253).str());
	_masterField->addChild(label, 30);
	label->setPosition(100, 200);*/


}

void IngameScene::organizeScene()
{
	auto actionManager = apHookActionManager::getInstance();
	auto showLogo = [this]() {
		auto logo = Sprite::create("elvanovLogo2.png");
		util.resizeSprite(logo, 480, true);
		auto visibleSize = Director::getInstance()->getVisibleSize();
		logo->setPosition(visibleSize.width / 2.f,visibleSize.height / 2.f);
		_masterField->addChild(logo, 1, "logo");
	};
	actionManager->addAction("game_start", "show_logo", showLogo);

	auto delLogo = [this]() {
		scheduleOnce([this](float delta)->void {
			_masterField->removeChildByName("logo");
			apHookActionManager::getInstance()->runHook("show_stage_select");
		}, 1.f, "delLogo");
	};
	actionManager->addAction("game_start", "del_logo", delLogo);

	auto displayStageSelect = [this]() {
		auto stageSelectNode = Node::create();
		_masterField->addChild(stageSelectNode, 1, "stageSelectNode");

		TTFConfig conf("fonts/SpoqaHsRegular.ttf", 12);
		auto label = STLabel::createWithTTF(conf, translate("stage").str() + " 1");
		auto labelSize = label->getContentSize();
		label->setPosition(300, 300);
		stageSelectNode->addChild(label);

		auto touchManager = APTouchManager::getInstance();
		touchManager->registerNode(label, APTouchManager::createCheckerWithRect(label,
			Rect(-labelSize.width / 2, -labelSize.height / 2, labelSize.width, labelSize.height)));
		touchManager->addHook(label, APTouchType::EndedIn, "stage_1_clicked");
	};
	actionManager->addAction("show_stage_select", "display_stage_select", displayStageSelect);

	auto stage1Start = [this]() {
		apHookActionManager::getInstance()
			->runHook("out_stage")
			->runHook("enter_stage1");
	};
	actionManager->addAction("stage_1_clicked", "stage_1_start", stage1Start);

	auto removeStage = [this]() {
		_masterField->removeChildByName("stageSelectNode");
	};
	actionManager->addAction("out_stage", "remove_stage", removeStage);

	auto displayStage1 = [this]() {
		initializeEffectManager();
		initializePhysics();
		debugVariable();
		gameInterface();
		animationTest();

	};
	actionManager->addAction("enter_stage1", "display_stage1", displayStage1);
	_debugBox->get() << actionManager->_d_all_hook() << DebugBox::push;
}

void IngameScene::debugVariable() {
	//-----------------------------------------
	// add variable settings editbox
	//------------------------------------------
	// set nowFieldText Position
	auto visibleSize = Director::getInstance()->getVisibleSize();
	_nowTextPos = Vec2(50.0f, visibleSize.height - 15.0f);

	/*addTextField([this](cocos2d::ui::EditBox* editbox)->void {
	this->setJumpPower(std::atof(editbox->getText()));
	}, "jumpPower");

	addTextField([this](EditBox* editbox)->void {
	world->SetGravity(b2Vec2(0, std::atof(editbox->getText())));
	}, "gravity");

	addTextField([this](EditBox* editbox)->void {
	_airResistance = std::atof(editbox->getText());
	_originAirResistance = _airResistance;
	}, "airResistance");
	*/
	addTextField([this](EditBox * editbox)->void {
		_timeScale = std::atof(editbox->getText());
		_localScheduler->setTimeScale(_timeScale);
	}, "timeScale");
	addTextField(&_movePower, "movePower");
	addTextField([this](EditBox * editbox)->void {
		_boxWidth = std::atof(editbox->getText());
		replaceBox();
	}, "boxWidth");
	addTextField([this](EditBox * editbox)->void {
		_boxHeight = std::atof(editbox->getText());
		replaceBox();
	}, "boxHeight");
	/*addTextField([this](EditBox * editbox)->void {
	_boxDensity = std::atof(editbox->getText());
	replaceBox();
	}, "boxDensity");
	addTextField([this](EditBox * editbox)->void {
	for(auto& item:_walls) {
	for (b2Fixture* f = item->GetFixtureList(); f; f = f->GetNext()) {
	f->SetFriction(std::atof(editbox->getText()));
	}
	}
	}, "wallFriction");
	addTextField([this](EditBox * editbox)->void {
	_cameraMoveSpeed = std::atof(editbox->getText());
	}, "cameraSpeed");*/
	addTextField([this](EditBox * editbox)->void {
		_dashPower = std::atof(editbox->getText());
	}, "dashPower");
	addTextField([this](EditBox * editbox)->void {
		_gravityScaleWhenJump = std::atof(editbox->getText());
	}, "gravityScaleWhenJump");
	addTextField([this](EditBox * editbox)->void {
		_dashAirRatio = std::atof(editbox->getText());
	}, "dashAirRatio");

	addTextField(&_cameraVRatio, "cameraVRatio");
	addTextField(&_cameraVDuration, "cameraVDuration");
	addTextField([this](EditBox * editbox)->void {
		_camera->setVibrationInterval(std::atof(editbox->getText()));
	}, "vibrationRatio");
	addTextField(&_dashWallBouncePower, "dashWallBouncePower");
	addTextField([this](EditBox * editbox)->void {
		_dashWallBounceRadian = std::atof(editbox->getText()) * M_PI;
	}, "dashWallBounceRadian");
	addTextField([this](EditBox * editbox)->void {
		_dashAngle = std::atof(editbox->getText()) * M_PI;
	}, "dasnAngle");
	addTextField(&_dashDuration, "dashDuration");
	addTextField([this](EditBox * editbox)->void {
		_effectConf.count = std::atoi(editbox->getText());
	}, "effectCount");
	addTextField(&(_effectConf.minPower), "pMinPower");
	addTextField(&(_effectConf.maxPower), "pMaxPower");
	addTextField(&(_effectConf.minSize), "minSize");
	addTextField(&(_effectConf.maxSize), "pmaxSize");
	addTextField(&(_effectConf.minGravityScale), "pminGravityScale");
	addTextField(&(_effectConf.maxGravityScale), "pmaxGravityScale");
	addTextField(&(_effectConf.minDuration), "pminDuration");
	addTextField(&(_effectConf.maxDuration), "pmaxDuration");
	addTextField(&(_effectConf.friction), "pfriction");
	addTextField(&(_effectConf.restitution), "prestitution");
	addTextField(&(_effectConf.diffuseX), "pdiffuseX");
	addTextField(&(_effectConf.diffuseY), "pdiffuseY");
	addTextField([this](EditBox * editbox)->void {
		_effectConf.color.r = std::atoi(editbox->getText());
	}, "color.r");
	addTextField([this](EditBox * editbox)->void {
		_effectConf.color.g = std::atoi(editbox->getText());
	}, "color.g");
	addTextField([this](EditBox * editbox)->void {
		_effectConf.color.b = std::atoi(editbox->getText());
	}, "color.b");
	addTextField([this](EditBox * editbox)->void {
		_effectConf.colorBrightnessRange = std::atoi(editbox->getText());
	}, "colorBrightnessRange");

	addTextField(&(_effectConf.minAlpha), "minAlpha");
	addTextField(&(_effectConf.maxAlpha), "maxAlpha");

	addTextField([this](EditBox * editbox)->void {
		_positionIterations = std::atoi(editbox->getText());
	}, "positionIterations");
	addTextField([this](EditBox * editbox)->void {
		_velocityIterations = std::atoi(editbox->getText());
	}, "_velocityIterations");
}
void IngameScene::animationTest()
{
	auto getAnimation = [](const char *format, int start, int end) -> cocos2d::Vector<SpriteFrame*>
	{
		auto spritecache = SpriteFrameCache::getInstance();
		Vector<SpriteFrame*> animFrames;
		char str[100];
		for (int i = start; i <= end; i++)
		{
			sprintf(str, format, i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
		}
		return animFrames;
	};
	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile("char.plist");
	auto frames = getAnimation("char%04d", 0, 15);
	auto animation = Animation::createWithSpriteFrames(frames, 1 / 15.f, 1);
	auto charanimationspr = Sprite::createWithSpriteFrame(frames.front());
	_masterField->addChild(charanimationspr);
	charanimationspr->setPosition(100, 100);
	charanimationspr->runAction(RepeatForever::create(Animate::create(animation)));


	spritecache->addSpriteFramesWithFile("charThree.plist");
	auto frames2 = getAnimation("charThree%04d", 0, 15);
	auto animation2 = Animation::createWithSpriteFrames(frames2, 1 / 15.f, 1);
	auto charanimationspr2 = Sprite::createWithSpriteFrame(frames2.front());
	_masterField->addChild(charanimationspr2);
	charanimationspr2->setPosition(200, 100);
	charanimationspr2->setScale(_boxWidth / charanimationspr2->getContentSize().width);
	charanimationspr2->runAction(RepeatForever::create(Animate::create(animation2)));


}
void IngameScene::gameInterface()
{


	auto buttonSize = Size(150.f, 150.f);
	auto touchManager = APTouchManager::getInstance();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	//---------------
	//	 move left button
	//---------------
	auto leftButton = Sprite::create("arrow-left.png");
	auto leftButtonSize = buttonSize;//leftButton->getContentSize();
	leftButton->setPosition(leftButtonSize.width / 2, leftButtonSize.height / 2);
	_camera->addChild(leftButton, 1, "leftButton");
	touchManager->registerNode(leftButton,
		APTouchManager::createCheckerWithRect(leftButton,
			Rect(-buttonSize.width / 2, -buttonSize.height / 2,
				buttonSize.width, buttonSize.height)));
	touchManager->setOrder(leftButton, 10);
	auto leftOn = [this]()->void {
		endDash();
		_checkMoveButton = true;
		_isRight = false;
	};
	auto rightOn = [this]()->void {
		endDash();
		_checkMoveButton = true;
		_isRight = true;
	};
	auto cancelMove = [this]()->void {
		this->endDash();
		_checkMoveButton = false;
	};
	touchManager->addBehavior(leftButton, APTouchType::Began, leftOn,
		"leftDown", "leftDown_b");
	touchManager->addBehavior(leftButton, APTouchType::MovedInner, leftOn,
		"leftMovedInner", "leftDown_b");
	touchManager->addBehavior(leftButton, APTouchType::MovedInnerIgnoreBegan,
		leftOn, "leftDownIgnoreBegan", "leftDown_b");
	touchManager->addBehavior(leftButton, APTouchType::EndedIn, cancelMove,
		"leftEndedIn", "leftDown_b");
	touchManager->addBehavior(leftButton, APTouchType::EndedInIgnoreBegan, cancelMove,
		"leftEndedInIgnoreBegan", "leftDown_b");
	touchManager->addBehavior(leftButton, APTouchType::EndedOut, cancelMove,
		"leftEndedOut", "leftDown_b");

	//---------------
	// move right button
	//---------------
	auto rightButton = Sprite::create("arrow-right.png");
	auto rightButtonSize = buttonSize;//rightButton->getContentSize();
	rightButton->setPosition(rightButtonSize.width * 1.5f, rightButtonSize.height / 2);
	_camera->addChild(rightButton, 1, "rightButton");
	touchManager->registerNode(rightButton,
		APTouchManager::createCheckerWithRect(rightButton,
			Rect(-buttonSize.width / 2, -buttonSize.height / 2,
				buttonSize.width, buttonSize.height)));
	touchManager->setOrder(rightButton, 10);
	touchManager->addBehavior(rightButton, APTouchType::Began, rightOn,
		"rightDown", "rightDown_b");
	touchManager->addBehavior(rightButton, APTouchType::MovedInner, rightOn,
		"rightMovedInner", "rightDown_b");
	touchManager->addBehavior(rightButton, APTouchType::MovedInnerIgnoreBegan,
		rightOn, "rightDownIgnoreBegan", "rightDown_b");
	touchManager->addBehavior(rightButton, APTouchType::EndedIn, cancelMove,
		"rightEndedIn", "rightDown_b");
	touchManager->addBehavior(rightButton, APTouchType::EndedInIgnoreBegan, cancelMove,
		"rightEndedInIgnoreBegan", "rightDown_b");
	touchManager->addBehavior(rightButton, APTouchType::EndedOut, cancelMove,
		"rightEndedOut", "rightDown_b");
	//	touchManager->addBehavior(rightButton, APTouchType::MovedOuter, cancelMove,
	//			"rightMovedOuter", "rightDown_b");
	//	touchManager->addBehavior(rightButton, APTouchType::MovedOuterIgnoreBegan,
	//			cancelMove, "rightMovedOuterIgnoreBegan", "rightDown_b");


	//---------------------
	// jump button
	//--------------------
	auto upButton = Sprite::create("arrow-up.png");
	auto upButtonSize = buttonSize;//upButton->getContentSize();
	upButton->setPosition(visibleSize.width - upButtonSize.width / 2, upButtonSize.height / 2);
	_camera->addChild(upButton, 1, "upButton");
	touchManager->registerNode(upButton,
		APTouchManager::createCheckerWithRect(upButton,
			Rect(-buttonSize.width / 2, -buttonSize.height / 2,
				buttonSize.width, buttonSize.height)));
	touchManager->setOrder(upButton, 10);
	touchManager->addBehavior(upButton, APTouchType::Began, [this]()->void {
		apHookActionManager::getInstance()->runHook("jump_clicked");
	}, "upDown", "upDownYeah");
	touchManager->addBehavior(upButton, APTouchType::EndedIn, [this]()->void {
		jumpTouchEnd();
	}, "upEndedIn", "upDownYeah");
	touchManager->addBehavior(upButton, APTouchType::EndedOut, [this]()->void {
		jumpTouchEnd();
	}, "upEndedOut", "upDownYeah");

	//---------------------
	// dash button
	//--------------------
	auto skillButton = Sprite::create("skill1.png");
	auto skillButtonSize = buttonSize;//skillButton->getContentSize();
	skillButton->setPosition(visibleSize.width - skillButtonSize.width *1.5f, skillButtonSize.height / 2);
	_camera->addChild(skillButton, 1, "skillButton");
	touchManager->registerNode(skillButton,
		APTouchManager::createCheckerWithRect(skillButton,
			Rect(-buttonSize.width / 2, -buttonSize.height / 2,
				buttonSize.width, buttonSize.height)));
	touchManager->setOrder(skillButton, 10);
	touchManager->addBehavior(skillButton, APTouchType::Began, [this]() {
		doDash();
	}, "skillDown", "skillDown_b");


	//=---------------------
	// Keyboard input
	//----------------------
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = [this, leftOn, rightOn](EventKeyboard::KeyCode keyCode, Event* event) {

		using K = EventKeyboard::KeyCode;
		switch (keyCode) {
		case K::KEY_A:
			_keyCount++;
			leftOn();
			break;
		case K::KEY_S:
			_keyCount++;
			rightOn();

			break;
		case K::KEY_K:
			doDash();
			break;
		case K::KEY_L:
			apHookActionManager::getInstance()->runHook("jump_clicked");
			break;
		default:
			break;

		}
		//cocos2d::log("Key with keycode %d pressed", keyCode);
	};
	keyboardListener->onKeyReleased = [this, cancelMove, leftOn, rightOn](EventKeyboard::KeyCode keyCode, Event* event) {
		using K = EventKeyboard::KeyCode;
		switch (keyCode) {
		case K::KEY_A:
			_keyCount--;
			if (_keyCount == 0) {
				cancelMove();
			}
			else if (_keyCount == 1) {
				rightOn();
			}
			break;

		case K::KEY_S:
			_keyCount--;
			if (_keyCount == 0) {
				cancelMove();
			}
			else if (_keyCount == 1) {
				leftOn();
			}
			break;
		case K::KEY_L:
			jumpTouchEnd();
		default:
			break;

		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	// background
	auto drawBack = DrawNode::create();
	drawBack->drawSolidRect(Vec2::ZERO, Vec2(visibleSize.width, visibleSize.height), Color4F(Color4B(84, 65, 52, 1)));
	this->addChild(drawBack, 0, "drawBack");

	// hookActions
	auto am = apHookActionManager::getInstance();
	am->addAction("jump_clicked", "do_jump", [this]()->void {
		// jump must exist only 1 until hit the ground.
		if (_floorFixtures.empty()) return;

		_charBody->SetLinearVelocity(b2Vec2(_charBody->GetLinearVelocity().x, 0));
		_charBody->ApplyLinearImpulse(b2Vec2(0, _jumpPower / SCALE_RATIO), _charBody->GetWorldCenter(), true);
		_charBody->SetGravityScale(_gravityScaleWhenJump);

		_checkJumpHighest = false;
	});

}
void IngameScene::initializePhysics()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto gravity = b2Vec2(0.0f, -40.0f);
	world = std::make_shared<b2World>(gravity);

	_debugDraw = std::make_shared<GLESDebugDraw>(SCALE_RATIO);
	world->SetDebugDraw(_debugDraw.get());
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	_debugDraw->SetFlags(flags);



	//ADD WALL
	

	_wallBuilder = STWallBuilder::create();
	_wallBuilder->makeWalls(_maps, "1");
	_masterField->addChild(_wallBuilder, 10, "wallBuilder");
	//_wallBuilder->makeWall(-20, -20, 40, 40);
	//_wallBuilder->makeWall(20, 0, 7, 10);


	/*addWall((visibleSize.width / 2), 50, visibleSize.width, 10); //CEIL
	addWall(0, (visibleSize.height / 2), 10, visibleSize.height); //LEFT
	addWall(visibleSize.width, (visibleSize.height / 2), 10,
		visibleSize.height); //RIGHT*/

							 // CHARACTER BOX2D PHYSICS!
	_sp = Sprite::create("character.png");
	_sp->setScaleX(_boxWidth / _sp->getContentSize().width);
	_sp->setScaleY(_boxHeight / _sp->getContentSize().height);
	_masterField->addChild(_sp);

	b2PolygonShape charShape;
	// box2d의 setbox는 가로세로가 아니라 반지름이랑 개념이 비슷하다. 가운데에서부터 한변 까지의 길이임. 그래서 /2를 해줌.
	charShape.SetAsBox(_boxWidth / SCALE_RATIO / 2, _boxHeight / SCALE_RATIO / 2);

	b2FixtureDef charFixtureDef;
	charFixtureDef.density = _boxDensity * (20.f * 32.f) / (_boxWidth * _boxHeight);
	charFixtureDef.friction = 0;
	charFixtureDef.restitution = 0;
	charFixtureDef.shape = &charShape;
	charFixtureDef.filter.categoryBits = CHARACTER;
	charFixtureDef.filter.maskBits = CHARACTER || BOSS || WALL;

	
	auto startCoord = _maps["level"]["1"]["start"];
	b2BodyDef charBodyDef;
	charBodyDef.position.Set((startCoord["x"] * OneBlockPx) / SCALE_RATIO, (startCoord["y"] * OneBlockPx) / SCALE_RATIO);
	charBodyDef.userData = _sp;
	charBodyDef.fixedRotation = true;
	charBodyDef.type = b2BodyType::b2_dynamicBody;

	_charBody = world->CreateBody(&charBodyDef);
	_charBody->CreateFixture(&charFixtureDef);

	// origin Air resistance setting.
	_originAirResistance = _airResistance;


	//----------------------------------
	// Character ContactListener
	//----------------------------------
	_cl_p = std::make_shared<STContactListener>(this);
	world->SetContactListener(_cl_p.get());


	_cl_p->setBeginContact(_charBody, [this](b2Contact* contact, b2Fixture* other) {
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		cocos2d::log("begContact normalX:%.2f, normalY:%.2f", worldManifold.normal.x,
			worldManifold.normal.y);
		if (worldManifold.normal.y == 1.0f) {
			_floorFixtures.emplace(other);
		}

	});

	_cl_p->setPostSolve(_charBody,
		[this](b2Contact* contact, const b2ContactImpulse* impulse, b2Fixture* other) {

		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);

		_hitPower = impulse->normalImpulses[0];
		// if hit Ground!
		if (impulse->normalImpulses[0] >= 0.05f &&
			worldManifold.normal.y == 1.0f &&
			worldManifold.normal.x == 0.0f) {

			/*cocos2d::log("nX:%.2f, nY:%.2f, inp0:%.2f, inp1:%.2f, itp0:%.2f, itp1:%.2f",
				worldManifold.normal.x,
				worldManifold.normal.y,
				impulse->normalImpulses[0],
				impulse->normalImpulses[1], // useless. -107374176.00
				impulse->tangentImpulses[0], // useless. -0.00
				impulse->tangentImpulses[1]); // useless. -107374176.00
				*/
											  // power max : 0.2f
			float power = impulse->normalImpulses[0];
			if (power >= 0.2f) {
				power = 0.2f;
			}
			//_characterHitGroundYes = true;
			characterHitGround(power);



		}
		// hit walls if character is moving left
		else if ((impulse->normalImpulses[0] >= 0.01f ||
			impulse->normalImpulses[1] >= 0.01f) &&
			worldManifold.normal.y == 0.0f &&
			worldManifold.normal.x == 1.0f) {
			characterHitLeftWall(impulse->normalImpulses[0]);
			//_characterHitLeftWallYes = true;
		}

		// hit walls if character is moving right
		else if ((impulse->normalImpulses[0] >= 0.01f ||
			impulse->normalImpulses[1] >= 0.01f) &&
			worldManifold.normal.y == 0.0f &&
			worldManifold.normal.x == -1.0f) {

			characterHitRightWall(impulse->normalImpulses[0]);
			//_characterHitRightWallYes = true;
		}

	});

	_cl_p->setEndContact(_charBody, [this](b2Contact* contact, b2Fixture* other) {
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		cocos2d::log("endContact normalX:%.2f, normalY:%.2f", worldManifold.normal.x,
			worldManifold.normal.y);
		if (_floorFixtures.find(other) != _floorFixtures.end()){
			_floorFixtures.erase(other);
		}
	});
	// if Hit Ground, Wall.
	_localUpdater->addFunc(P_WALLCHECK, [this](float)->void {
		if (_characterHitGroundYes) {
			_characterHitGroundYes = false;
			characterHitGround(_hitPower);

		}
		else if (_characterHitLeftWallYes) {
			_characterHitLeftWallYes = false;
			characterHitLeftWall(_hitPower);
		}
		else if (_characterHitRightWallYes) {
			_characterHitRightWallYes = false;
			characterHitRightWall(_hitPower);
		}
	
	});

	// physical things.
	_localUpdater->addFunc(P_WORLDSTEP, [this](float delta)->void {
		// moveButton 1
		auto movePowerToApply = delta * _movePower / SCALE_RATIO;
		if (_checkMoveButton) {
			float isRight = 1.0f;
			if (!_isRight) {
				isRight = -1.0f;
			}

			_charBody->ApplyLinearImpulse(b2Vec2(isRight * movePowerToApply, 0), _charBody->GetWorldCenter(), true);

		}
		// air resistance. 2
		auto v = _charBody->GetLinearVelocity().x;
		if (v >= 0.0001f || v <= -0.0001f) {
			float airResistanceDirection = 1.0f;
			if (v >= 0) airResistanceDirection = -1.0f;
			float resistanceToApply = airResistanceDirection * std::abs(v) * _airResistance * delta * 60.f / SCALE_RATIO;
			//cocos2d::log("resistanceToApply: %f", resistanceToApply);
			if (std::abs(resistanceToApply) >= std::abs(movePowerToApply) * 2.f) {
				resistanceToApply = airResistanceDirection * movePowerToApply * 2.f;
			}
			_charBody->ApplyLinearImpulse(b2Vec2(resistanceToApply, 0), _charBody->GetWorldCenter(), true);
		}

		// jumpEnd Check (while long 1)
		if ((!_checkJumpHighest) && _charBody->GetLinearVelocity().y <= 0) {
			_checkJumpHighest = true;
			jumpTouchEnd();
		}


		// world step.
		world->Step(delta, _velocityIterations, _positionIterations);


		// update node position
		for (b2Body *body = world->GetBodyList(); body != NULL; body =
			body->GetNext())
			if (body->GetUserData()) {
				auto sprite = (Node*)body->GetUserData();
				sprite->setPosition(
					Vec2(body->GetPosition().x * SCALE_RATIO,
						body->GetPosition().y * SCALE_RATIO));
				sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(body->GetAngle()));

			}

		//clear forces 
		world->ClearForces();
	});

	// camera
	_localUpdater->addFunc(P_CAMERA, [this](float delta) {
		// camera update with vibration offset. 4
		auto camPos = _camera->getCameraPosition();
		//_camera->setVibrationOffset(_vibrationOffset);
		auto posToMove = _sp->getPosition();
		_camera->setScale(2.0);
		//auto size
		//if(posToMove.x < )
		_camera->setCameraPosition(camPos + (_sp->getPosition() - camPos) * _cameraMoveSpeed);
		//_camera->updateCamera(delta);
	});

	/*_localUpdater->addFunc(213, [this](float delta) {
		for (auto& item : _floorFixtures) {
			_debugBox->get() << std::hex << (int)item << ", ";
		}
		_debugBox->get() << DebugBox::push;
	});*/
	
}
void IngameScene::initializeEffectManager()
{
	//---------------------
	// effect manager
	//---------------------
	_effectGen = STEffectGenerator::create();
	_masterField->addChild(_effectGen);


	/// effect setting!!///
	_effectConf.count = 10;
	_effectConf.minPower = 0.5f; // 0.5
	_effectConf.maxPower = 2.5f; // 2.5
	_effectConf.minSize = 1.f;
	_effectConf.maxSize = 7.f;
	_effectConf.minGravityScale = 0.8f; // 0.8
	_effectConf.maxGravityScale = 0.8f; // 0.8
	_effectConf.minDuration = 3.f;
	_effectConf.maxDuration = 5.f;
	_effectConf.restitution = 0.4f;
	_effectConf.friction = 0.2f; // 0.2
	_effectConf.diffuseX = 40.f;
	_effectConf.diffuseY = 0.f;
	_effectConf.color = cocos2d::Color3B(107, 88, 71);
	_effectConf.minAlpha = 200.f;
	_effectConf.maxAlpha = 200.f;
	_effectConf.colorBrightnessRange = 30;
	_effectConf.fixedRotation = false;
	_effectConf.type = STEffectType::Boom;
}

void IngameScene::jumpTouchEnd() {
	_charBody->SetGravityScale(1.0f);

}

void IngameScene::doDash() {

	if (!_checkMoveButton) return;

	float direction = 1.0f;
	float finalDashAngle = 0;
	if(_isRight == true) finalDashAngle = _dashAngle;
	else finalDashAngle = M_PI - _dashAngle;
	_charBody->SetLinearVelocity(b2Vec2(0.f, /*_charBody->GetLinearVelocity().y*/0.f));
	_charBody->ApplyLinearImpulse(b2Vec2(
		_dashPower * std::cos(finalDashAngle) / SCALE_RATIO, 
		_dashPower * std::sin(finalDashAngle) / SCALE_RATIO), _charBody->GetWorldCenter(), true);


	_airResistance *= _dashAirRatio;
	_isDashing = true;


	unscheduleLocally("endDash");
	scheduleOnceLocally([this](float delta)->void {endDash(); },_dashDuration, "endDash");
}

void IngameScene::endDash() {
	_isDashing = false;
	unscheduleLocally("endDash");
	_airResistance = _originAirResistance;

}
void IngameScene::addTextField(const std::function<void(cocos2d::ui::EditBox*)>& callback, const std::string& placeHolder) {

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto textfieldHeight = 20.f;
	auto textfieldWidth = 100.f;
	auto editBox = STTextField::create(callback, cocos2d::Size(100.0f, textfieldHeight), placeHolder);
	_camera->addChild(editBox, 20);
	editBox->setPosition(_nowTextPos);
	_nowTextPos.x += textfieldWidth;
	if(_nowTextPos.x >= visibleSize.width - textfieldWidth / 2.f) {
		_nowTextPos.x = textfieldWidth / 2.f;
		_nowTextPos.y -= textfieldHeight;
	}



}

void IngameScene::addTextField(float* fp, const std::string& placeHolder) {
	addTextField([fp](EditBox * editbox)->void {
		*fp = std::atof(editbox->getText());
	}, placeHolder);
}
void IngameScene::replaceBox() {

	b2PolygonShape charShape;
	charShape.SetAsBox(_boxWidth / SCALE_RATIO ,_boxHeight / SCALE_RATIO);

	b2FixtureDef charFixtureDef;
	charFixtureDef.density = _boxDensity * (20.f * 32.f) / (_boxWidth * _boxHeight);
	charFixtureDef.friction = 0.5;
	charFixtureDef.restitution = 0;
	charFixtureDef.shape = &charShape;


//	b2BodyDef charBodyDef;
//	charBodyDef.position.Set((visibleSize.width / 2) / SCALE_RATIO, (visibleSize.height / 2)/SCALE_RATIO);
//	charBodyDef.userData = _sp;
//	charBodyDef.fixedRotation = true;
//	charBodyDef.type = b2BodyType::b2_dynamicBody;

//	_charBody = _world->CreateBody(&charBodyDef);
	for (b2Fixture* f = _charBody->GetFixtureList(); f;) {

		auto fixtureToDestroy = f;
		f = f->GetNext();
		_charBody->DestroyFixture(fixtureToDestroy);

	}
	_charBody->CreateFixture(&charFixtureDef);

	_sp->setScaleX(_boxWidth / 5);
	_sp->setScaleY(_boxHeight / 5);
}
std::shared_ptr<b2World> IngameScene::getb2World()
{
	return IngameScene::world;
}
void IngameScene::setJumpPower(float power) {
	_jumpPower=power;
	cocos2d::log("%f",power);
}

void IngameScene::addWall(float px, float py, float w, float h) {
	b2PolygonShape floorShape;

	floorShape.SetAsBox(w / SCALE_RATIO / 2, h / SCALE_RATIO / 2);

	b2FixtureDef floorFixture;
	floorFixture.density = 1.0f;
	floorFixture.friction = 0.5f;
	floorFixture.restitution = 0;
	floorFixture.shape = &floorShape;

	auto sp = Sprite::create("bg.png");
	sp->setScaleX(w * 0.1f);
	sp->setScaleY(h * 0.1f);
	_masterField->addChild(sp, 0);

	b2BodyDef floorBodyDef;
	floorBodyDef.position.Set(px / SCALE_RATIO, py / SCALE_RATIO);
	floorBodyDef.userData = sp;


	auto wallBody = world->CreateBody(&floorBodyDef);
	wallBody->CreateFixture(&floorFixture);
	_walls.emplace_back(wallBody);

}

void IngameScene::update(float delta) {

	// timeScale based delta time.
	//this->getScheduler()->setTimeScale(_timeScale);
	// load local scheduler.
	_localScheduler->update(delta);
	//_localScheduler->setTimeScale(_timeScale);

	// debug velocity pixel per frame
	/*
	if (_checkMoveButton) {
		auto v = _charBody->GetLinearVelocity();
		cocos2d::log("x velocity: %f, y velocity: %f", v.x * SCALE_RATIO, v.y  * SCALE_RATIO);
	}*/

	// do tasks within forced time
	apAsyncTaskManager::getInstance()->doTasks();
}

void IngameScene::characterHitGround(float power) {
	_camera->vibrateCameraDirection(
		power * _cameraVRatio,
		power * _cameraVDuration,
		0.5f * M_PI);

	_haveToGenerateEffect = true;
	cocos2d::log("hit the ground power is %f", power);
	boost::coroutines::symmetric_coroutine<void>::call_type gogo(
		[this](boost::coroutines::symmetric_coroutine<void>::yield_type& yield) {
		_effectGen->generateEffect(_effectConf, _sp->getPosition().x, _sp->getPosition().y - _boxHeight / 2);
	});
	apAsyncTaskManager::getInstance()->addTask(std::move(gogo));
	
	// Sound Test
	auto ae = CocosDenshion::SimpleAudioEngine::getInstance();
	//ae->preloadEffect("sounds/stone2.ogg");
	//ae->playEffect("sounds/stone2.ogg", false);
	//ae->playEffect("sounds/stone.mp3");
	//ae->playEffect("sounds/stone.wav");

}

void IngameScene::characterHitLeftWall(float power) {
	cocos2d::log("leftWallHitPower : %f", power);
	_debugBox->get() << power << DebugBox::push;
	// wall bouncing
	if (_isDashing == true) {
		auto bouncePower = _dashWallBouncePower * (1.0f - _dashWallCondition + power);
		_charBody->ApplyLinearImpulse(
			b2Vec2(
				std::cos(_dashWallBounceRadian) * bouncePower / SCALE_RATIO,
				std::sin(_dashWallBounceRadian) * bouncePower / SCALE_RATIO),
			_charBody->GetWorldCenter(), true);
		
	}
	endDash();
}
void IngameScene::characterHitRightWall(float power) {
	cocos2d::log("rightWallHitPower : %f", power);

	// wall bouncing
	if (_isDashing == true) {
		auto bouncePower = _dashWallBouncePower * (1.0f - _dashWallCondition + power);

		_charBody->ApplyLinearImpulse(
			b2Vec2(
				std::cos(M_PI - _dashWallBounceRadian) * bouncePower / SCALE_RATIO,
				std::sin(M_PI - _dashWallBounceRadian) * bouncePower / SCALE_RATIO),
			_charBody->GetWorldCenter(), true);
	}
	endDash();
}


void IngameScene::draw(Renderer *renderer, const Mat4& transform, uint32_t transformUpdated) {
	
	glEnableVertexAttribArray(0);
	if (world) {
		world->DrawDebugData();
	}

}



IngameScene * IngameScene::getInstance()
{
	return _uniqueScene;
}

b2Body* IngameScene::getCharBody() {
	return _charBody;
}

std::vector<b2Body*> IngameScene::getWalls() {
	return _walls;
}

void IngameScene::scheduleLocally(const std::function<void(float)>& callback, float interval, unsigned int repeat, float delay, const std::string & key)
{
	_localScheduler->schedule(callback, this, interval, repeat, delay, !isRunning(), key);
}

void IngameScene::unscheduleLocally(const std::string & key)
{
	_localScheduler->unschedule(key, this);
}

void IngameScene::scheduleOnceLocally(const std::function<void(float)>& callback, float delay, const std::string & key)
{
	_localScheduler->schedule(callback, this, 0, 0, delay, !isRunning(), key);
}

Action * IngameScene::runLocalAction(Node* target, Action* action)
{
	CCASSERT(action != nullptr, "Argument must be non-nil");
	_localActionManager->addAction(action, target, !target->isRunning());
	return action;
}

IngameScene::IngameScene() {
	// TODO Auto-generated constructor stub

}

IngameScene::~IngameScene() {

	CC_SAFE_RELEASE(_localScheduler);
	CC_SAFE_RELEASE(_localActionManager);
	// TODO Auto-generated destructor stub
}

float IngameScene::Util::bySameRatio(float aStart, float a, float aEnd, float bStart, float bEnd)
{
	if (aStart > a || aEnd < a) return 0.f;
	if (aStart > aEnd || bStart > bEnd) return 0.f;

	auto aRatio = (a - aStart) / (aEnd - aStart);
	return bStart + (bEnd - bStart) * aRatio;
}

void IngameScene::Util::resizeSprite(cocos2d::Sprite * sprite, float side, bool sideIsWidth)
{
	auto size = sprite->getContentSize();
	auto l = 1.0f;
	if (sideIsWidth) {
		l = size.width;
	}
	else {
		l = size.height;
	}
	sprite->setScale(side / l);
}

IngameScene::Util::Util()
{
}

IngameScene::Util::~Util()
{
}

void IngameScene::UpdateCaller::initWithScheduler(cocos2d::Scheduler* s, const std::string& key)
{
	auto f = [this, key](float delta)->void {
		for (auto& item : _list) {
			auto func = item.second;
			func(delta);
		}
		//cocos2d::log("endLoopUpdateCaller : %s", key.c_str());
	};
	s->schedule(f, this, 0, false, key);
}

void IngameScene::UpdateCaller::delFunc(int priority)
{

	if (_list.find(priority) != _list.end()) {
		_list.erase(priority);
		return;
	}
	cocos2d::log("UpdateCaller-delFunc - key doesn't exist!");

}

IngameScene::UpdateCaller::UpdateCaller()
{
}

IngameScene::UpdateCaller::~UpdateCaller()
{
}

