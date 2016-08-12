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
#include "STBox2dNode.h"
#include "apAnimationManager.h"
#include "function_traits.h"
#include <sstream>
#include <future>
#include <fstream>
#include <random>

USING_NS_CC;

using namespace std;
float IngameScene::_timeScale = 1.0f;
std::shared_ptr<b2World> IngameScene::world = nullptr;
std::shared_ptr<STContactListener> IngameScene::_cl_p = nullptr;
const float IngameScene::OneBlockPx = 20.f;
IngameScene* IngameScene::_uniqueScene = nullptr;
ActionManager* IngameScene::_localActionManager = nullptr;
Scheduler* IngameScene::_localScheduler = nullptr;

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


	// custom Scheduler initializing.
	_localScheduler = new (std::nothrow) Scheduler();
	// action manager
	_localActionManager = new (std::nothrow) ActionManager();
	_localScheduler->scheduleUpdate(_localActionManager, Scheduler::PRIORITY_NON_SYSTEM_MIN, false);

	// updateCaller initialize.
	_localUpdater = make_shared<UpdateCaller>();
	_globalUpdater = make_shared<UpdateCaller>();
	_localUpdater->initWithScheduler(_localScheduler, "localUpdater");
	_globalUpdater->initWithScheduler(getScheduler(), "globalScheduler");
	boost::coroutines::symmetric_coroutine<void>::call_type updateCallFuncs(
			[this](boost::coroutines::symmetric_coroutine<void>::yield_type& yield) {
	});

	
	// Json load
	auto js = fu->getStringFromFile("map3.json");
	_maps.Parse(js.c_str());


	// initialize member variables.
	_boxWidth = 30.f;
	_boxHeight = 48.f;

	//camera and master field initialize
	_camera = STCamera::create();
	auto touchManager = APTouchManager::getInstance();
	_masterField = Node::create();
	_box2dWorld = Node::create();
	_camera->setField(_masterField);
	_backgroundField = Node::create();
	
	//_camera->setContentSize(visibleSize);
	this->addChild(_camera, 5, "camera");
	this->addChild(_masterField, 3, "masterField");
	this->addChild(_backgroundField, 1, "ingameBgNode");
	_masterField->addChild(_box2dWorld, 0, "box2dWorld");

	//color setting
	_colors["default"] = Color4B(255, 255, 255, 255);
	_colors["bg1-1"] = Color4B(76, 44, 40, 255);
	_colors["bg1-2"] = Color4B(175, 110, 98, 255);
	_colors["bg1-3"] = Color4B(255, 214, 204, 255);

	//image or resource path setting.
	_path["rightButton"] = "img/button_right.png";
	_path["leftButton"] = "img/button_left.png";
	_path["jumpButton"] = "img/button_jump.png";
	_path["skillButton"] = "img/button_skill.png";



	// font test
	/*TTFConfig conf("fonts/SpoqaHsRegular.ttf", 24);
	auto label = STLabel::createWithTTF(conf, (format(translate("You are so fast by speed {1}.")) % 4.253).str());
	_masterField->addChild(label, 30);
	label->setPosition(100, 200);*/


}

void IngameScene::organizeScene()
{
	auto actionManager = apHookActionManager::getInstance();
	auto defaultBackground = [this]() {
		auto size = Director::getInstance()->getVisibleSize();
		auto drawNode = DrawNode::create();
		drawNode->drawSolidRect(Vec2::ZERO, Vec2(size.width, size.height), util.switchColor(_colors["default"]));
		_backgroundField->addChild(drawNode, 0, "default_background");
	};
	//show Logo
	auto showLogo = [this]() {
		auto logo = Sprite::create("elvanovLogo2.png");
		logo->setOpacity(0);
		util.resizeSprite(logo, 320, true);
		auto visibleSize = Director::getInstance()->getVisibleSize();
		logo->setPosition(visibleSize.width / 2.f,visibleSize.height / 2.f);
		_masterField->addChild(logo, 1, "logo");

		auto delay1 = DelayTime::create(1);
		auto fadeTo = FadeTo::create(2, 180);
		auto sequence = Sequence::create(delay1, fadeTo, nullptr);
		logo->runAction(sequence);

	};
	actionManager->addAction("game_start", "show_logo", showLogo);
	actionManager->addAction("game_start", "default_background", defaultBackground);


	auto delLogo = [this]() {
		scheduleOnce([this](float delta)->void {
			_masterField->removeChildByName("logo");
			apHookActionManager::getInstance()->runHook("show_stage_select");
		}, 5.f, "delLogo");
	};
	actionManager->addAction("game_start", "del_logo", std::move(delLogo));

	// display stage select scene.
	auto displayStageSelect = [this]() {
		auto stageSelectNode = Node::create();
		_masterField->addChild(stageSelectNode, 1, "stageSelectNode");

		TTFConfig conf("fonts/SpoqaHsRegular.ttf", 12);
		auto label = STLabel::createWithTTF(conf, "STAGE1");
		auto labelSize = label->getContentSize();
		label->setPosition(300, 300);
		label->setTextColor(Color4B(0,0,0,255));
		stageSelectNode->addChild(label);

		auto touchManager = APTouchManager::getInstance();
		touchManager->registerNode(label, APTouchManager::createCheckerWithRect(label,
			Rect(-labelSize.width / 2, -labelSize.height / 2, labelSize.width, labelSize.height)));
		touchManager->addHook(label, APTouchType::EndedIn, "stage_1_clicked");
	};
	actionManager->addAction("show_stage_select", "display_stage_select", std::move(displayStageSelect));

	// stage 1 Start Action
	auto stage1Start = [this]() {
		apHookActionManager::getInstance()
			->runHook("out_stage")
			->runHook("enter_stage1");
	};
	actionManager->addAction("stage_1_clicked", "stage_1_start", std::move(stage1Start));

	// remove Stage.
	auto removeStage = [this]() {
		_masterField->removeChildByName("stageSelectNode");
	};
	actionManager->addAction("out_stage", "remove_stage", std::move(removeStage));

	// display Stage 1
	auto displayStage1 = [this]() {

		initializeEffectManager();
		initializePhysics("1");
		debugVariable();
		gameInterface();
		animationTest();
		apHookActionManager::getInstance()
			->runHook("after_display_stage1");
	};
	actionManager->addAction("enter_stage1", "display_stage1", std::move(displayStage1));
	
	/*auto debugRefreshPics = [this]() {
		std::map<std::string, std::string> m;
		m["leftButton"] = "arrow-left.png";
		m["rightButton"] = "arrow-right.png";
		m["upButton"] = "arrow-up.png";
		m["skillButton"] = "skill1.png";
		schedule([this, m](float delta) {
			for (auto& item : m) {
				auto& name = item.first;
				auto& filename = item.second;
				auto b = _camera->getChildByName<Sprite*>(name);
				auto fileExists = cocos2d::FileUtils::getInstance()->isFileExist(filename) == true;
				//Director::getInstance()->getTextureCache()->add
				if(b && fileExists) b->initWithFile(filename);
			}
			
		},1, CC_REPEAT_FOREVER, 0, "refresh_pics");
	};
	actionManager->addAction("after_display_stage1", "refresh_pics", debugRefreshPics);*/
	
	
	
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
	charanimationspr->setPosition(300, 300);
	//charanimationspr->runAction(RepeatForever::create(Animate::create(animation)));


	spritecache->addSpriteFramesWithFile("charThree.plist");
	auto frames2 = getAnimation("charThree%04d", 0, 15);
	auto animation2 = Animation::createWithSpriteFrames(frames2, 1 / 15.f, 1);
	auto charanimationspr2 = Sprite::createWithSpriteFrame(frames2.front());
	_masterField->addChild(charanimationspr2);
	charanimationspr2->setPosition(300, 400);
	charanimationspr2->setScale(_boxWidth / charanimationspr2->getContentSize().width);
	//charanimationspr2->runAction(RepeatForever::create(Animate::create(animation2)));


}
void IngameScene::gameInterface()
{

	auto buttonSize = Size(150.f, 150.f);
	auto buttonSpriteSize = Size(110.f, 110.f);
	auto touchManager = APTouchManager::getInstance();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	//---------------
	//	 move left button
	//---------------
	auto leftButton = Sprite::create(_path["leftButton"]);
	leftButton->setPosition(buttonSize.width / 2, buttonSize.height / 2); // 75,75
	leftButton->setScale(buttonSpriteSize.width / leftButton->getContentSize().width);
	_camera->addChild(leftButton, 10, "leftButton");
	/*touchManager->registerNode(leftButton,
		APTouchManager::createCheckerWithRect(leftButton,
			Rect(-buttonSize.width / 2, -buttonSize.height / 2,
				buttonSize.width, buttonSize.height)));*/ // (0,0) ~ (150,150)
	touchManager->registerNode(leftButton,
		APTouchManager::createCheckerWithRect(
			Rect(0, 0,
				buttonSize.width, buttonSize.height)));
	
	touchManager->setOrder(leftButton, 10);
	auto leftOn = [this]()->void {
		//endDash();
		_checkMoveButton = true;
		_isRight = false;
	};
	auto rightOn = [this]()->void {
		//endDash();
		_checkMoveButton = true;
		_isRight = true;
	};
	auto cancelMove = [this]()->void {
		apHookActionManager::getInstance()->runHook("dash_end");
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
	auto rightButton = Sprite::create(_path["rightButton"]);
	rightButton->setPosition(buttonSize.width * 1.5f, buttonSize.height / 2);
	rightButton->setScale(buttonSpriteSize.width / rightButton->getContentSize().width);
	_camera->addChild(rightButton, 10, "rightButton");
	/*touchManager->registerNode(rightButton,
		APTouchManager::createCheckerWithRect(rightButton,
			Rect(-buttonSize.width / 2, -buttonSize.height / 2,
				buttonSize.width, buttonSize.height)));*/
	touchManager->registerNode(rightButton,
		APTouchManager::createCheckerWithRect(
			Rect(buttonSize.width, 0,buttonSize.width, buttonSize.height)));
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
	auto upButton = Sprite::create(_path["jumpButton"]);
	auto upButtonSize = buttonSize;//upButton->getContentSize();
	upButton->setPosition(visibleSize.width - upButtonSize.width / 2, upButtonSize.height / 2);
	upButton->setScale(buttonSpriteSize.width / upButton->getContentSize().width);
	_camera->addChild(upButton, 10, "upButton");
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
	auto skillButton = Sprite::create(_path["skillButton"]);
	auto skillButtonSize = buttonSize;//skillButton->getContentSize();
	skillButton->setPosition(visibleSize.width - skillButtonSize.width *1.5f, skillButtonSize.height / 2);
	skillButton->setScale(buttonSpriteSize.width / skillButton->getContentSize().width);
	_camera->addChild(skillButton, 10, "skillButton");
	touchManager->registerNode(skillButton,
		APTouchManager::createCheckerWithRect(skillButton,
			Rect(-buttonSize.width / 2, -buttonSize.height / 2,
				buttonSize.width, buttonSize.height)));
	touchManager->setOrder(skillButton, 10);
	touchManager->addBehavior(skillButton, APTouchType::Began, [this]() {
		apHookActionManager::getInstance()->runHook("skill_clicked");
		jumpTouchEnd();
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
			apHookActionManager::getInstance()->runHook("skill_clicked");
			break;
		case K::KEY_L:
			apHookActionManager::getInstance()->runHook("jump_clicked");
			break;
		default:
			break;

		}
		/*auto animationWalk = util.getAnimation("chartest%04d", _mcFrameRanges["walk"]);
		runLocalAction(_sp, Animate::create(animationWalk));

		static_cast<Sprite*>(_sp)->setSpriteFrame(frameToDisplay);
		*/

		auto aniManager = apAnimationManager::getInstance();
		aniManager->playAnimation(_sp, "walk", false);
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
	/*
	auto drawBack = DrawNode::create();
	drawBack->drawSolidRect(Vec2::ZERO, Vec2(visibleSize.width, visibleSize.height), Color4F(Color4B(84, 65, 52, 1)));
	this->addChild(drawBack, 0, "drawBack");*/

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

	//skill(dash) button clicked
	am->addAction("skill_clicked", "do_skill", [this]()->void {

		jumpTouchEnd();
		
		if (!_checkMoveButton) return;

		float direction = 1.0f;
		float finalDashAngle = 0;
		if (_isRight == true) finalDashAngle = _dashAngle;
		else finalDashAngle = M_PI - _dashAngle;
		_charBody->SetLinearVelocity(b2Vec2(0.f, /*_charBody->GetLinearVelocity().y*/0.f));
		_charBody->ApplyLinearImpulse(b2Vec2(
			_dashPower * std::cos(finalDashAngle) / SCALE_RATIO,
			_dashPower * std::sin(finalDashAngle) / SCALE_RATIO), _charBody->GetWorldCenter(), true);


		_airResistance *= _dashAirRatio;
		_isDashing = true;


		unscheduleLocally("endDash");
		scheduleOnceLocally([this](float delta)->void {
			apHookActionManager::getInstance()->runHook("dash_end");
		}, _dashDuration, "endDash");
	});

	//dash end event
	am->addAction("dash_end", "do_dash_end", [this]()->void {
		_isDashing = false;
		unscheduleLocally("endDash");
		_airResistance = _originAirResistance;
	});

	//bounce when you hit the wall while dashing
	auto bounce = [this](float power, bool isRight) ->void {
		_debugBox->get() << "bounced!!" << DebugBox::push;
		auto bouncePower = _dashWallBouncePower * (1.0f - _dashWallCondition + power);
		float radian = 0;
		if (!isRight) {
			radian = _dashWallBounceRadian;
		}
		else {
			radian = M_PI - _dashWallBounceRadian;
		}

		_charBody->ApplyLinearImpulse(
			b2Vec2(
				std::cos(radian) * bouncePower / SCALE_RATIO,
				std::sin(radian) * bouncePower / SCALE_RATIO),
			_charBody->GetWorldCenter(), true);
	};
	am->addAction("character_dash_wall_right", "bounce_right", [this, bounce](float power)->void {
		bounce(power, true);
	});
	am->addAction("character_dash_wall_left", "bounce_left", [this, bounce](float power)->void {
		bounce(power, false);
	});

	am->addAction("character_hit_ground", "camera_vibration_and_effect", [this](float power)->void {
		_camera->vibrateCameraDirection(
			power * _cameraVRatio,
			power * _cameraVDuration,
			0.5f * M_PI);

		cocos2d::log("hit the ground power is %f", power);
		boost::coroutines::symmetric_coroutine<void>::call_type gogo(
			[this](boost::coroutines::symmetric_coroutine<void>::yield_type& yield) {
			_effectGen->generateEffect(_effectConf, _characterNode->getPosition().x, _characterNode->getPosition().y - _boxHeight / 2);
		});
		apAsyncTaskManager::getInstance()->addTask(std::move(gogo));
	
	});

}
void IngameScene::initializePhysics(const std::string& level)
{
	_level = level;
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto gravity = b2Vec2(0.0f, -40.0f);
	world = std::make_shared<b2World>(gravity);

	_cl_p = std::make_shared<STContactListener>(this);
	world->SetContactListener(_cl_p.get());

#if  (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_debugDraw = std::make_shared<GLESDebugDraw>(SCALE_RATIO);
	world->SetDebugDraw(_debugDraw.get());
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	_debugDraw->SetFlags(flags);
#endif

	// ---------------------------------------------------
	// -------------------ADD WALL------------------------
	// ---------------------------------------------------

	_wallBuilder = STWallBuilder::create();
	_wallBuilder->makeWalls(_maps, _level);
	_camera->setBorderSize(
		_maps["level"][_level.c_str()]["size"]["w"].GetInt(),
		_maps["level"][_level.c_str()]["size"]["h"].GetInt());
	_masterField->addChild(_wallBuilder, 10, "wallBuilder");

	//_wallBuilder->makeWall(-20, -20, 40, 40);
	//_wallBuilder->makeWall(20, 0, 7, 10);
	/*addWall((visibleSize.width / 2), 50, visibleSize.width, 10); //CEIL
	addWall(0, (visibleSize.height / 2), 10, visibleSize.height); //LEFT
	addWall(visibleSize.width, (visibleSize.height / 2), 10,
		visibleSize.height); //RIGHT*/


	//add backgrounds
	

	using scv = boost::coroutines::symmetric_coroutine<void>;
	
	apAsyncTaskManager::getInstance()->addTask(
		scv::call_type([this](scv::yield_type& yield) {
		_backgroundFollowRatio.clear();
		_debugBox->get() << "abcedfg" << _debugBox->push;

		if (_level == "1") {
			auto makeBackground = [this, &yield](
				float minHeight, float maxHeight, float minLength, float maxLength, Color4B color)
			-> cocos2d::Node* {
				auto drawNode = DrawNode::create();
				auto totalLength = 0.f;
				auto lengthLimit = Director::getInstance()->getVisibleSize().width;
				std::vector<std::tuple<float, float>> vs;
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<float>
					d_height(minHeight, maxHeight),
					d_length(minLength, maxLength);
				

				while (totalLength <= lengthLimit) {
					auto l = d_length(gen);
					auto h = d_height(gen);

					totalLength += l;
					if (totalLength > lengthLimit) {
						l -= totalLength - lengthLimit;
					}

					vs.emplace_back(make_tuple(l, h));
					//_debugBox->get() << "length : " << l << " height : " << h << _debugBox->push;
				}
				
				auto drawnode = DrawNode::create();
				totalLength = 0;
				auto n = [this, &vs, &drawnode, &totalLength, &color, &yield]() {
					for (auto& item : vs) {
						auto& length = std::get<0>(item);
						auto& height = std::get<1>(item);
						drawnode->drawSolidRect(
							Vec2(totalLength, 0.f),
							Vec2(totalLength + length, height),
							util.switchColor(color));
						totalLength += length;
					}
				};
				n();
				n();
				return drawnode;
			};

			auto bg3 = makeBackground(200.f, 350.f, 20.f, 200.f, _colors["bg1-3"]);
			auto bg2 = makeBackground(300.f, 350.f, 4.f, 100.f, _colors["bg1-2"]);
			auto bg1 = DrawNode::create();
			auto size = Director::getInstance()->getVisibleSize();
			bg1->drawSolidRect(Vec2(0, 0), Vec2(size.width, size.height), util.switchColor(_colors["bg1-1"]));

			auto bgSprite = Sprite::create("img/background3.png");
			bgSprite->setAnchorPoint(Vec2::ZERO);
			_backgroundField->addChild(bgSprite, 1);
			_backgroundField->addChild(bg3, 5);
			_backgroundField->addChild(bg2, 3);
			//_backgroundField->addChild(bg1, 1);
			_backgroundFollowRatio.emplace(bg3, 0.3f);
			_backgroundFollowRatio.emplace(bg2, 0.1f);
			//_backgroundFollowRatio.emplace(bg1, 0.f);
			_backgroundFollowRatio.emplace(bgSprite, 0.f);
			
			
			
		}


	}));


	// CHARACTER BOX2D PHYSICS!
	// actual sprite box width: 80px, height : 128px.

	// set MainCharacter animation Frames
	_mcFrameRanges["walk"] = FrameRange(0, 19);
	_mcFrameEvents["walk"][5] = "power_frame_event";
	_mcFramePath = "charTest1-1.plist";
	
	auto spritecache = SpriteFrameCache::getInstance();
	//spritecache->addSpriteFramesWithFile(_mcFramePath);

	//auto animationWalk = util.getAnimation("chartest%04d", _mcFrameRanges["walk"]);
	//animationWalk->getFrames().front()->
	//auto& animationWalkFrames = animationWalk->getFrames();
	


	//make animation
	auto aniManager = apAnimationManager::getInstance();
	aniManager->connectWithScheduler(_localScheduler);
	aniManager->addNewSpriteSheet(_mcFramePath);
	aniManager->specifyAnimation("chartest%04d", _mcFrameRanges["walk"], "walk");
	for (auto& item : _mcFrameEvents) {
		auto& animationName = item.first;
		for (auto& item2 : item.second) {
			auto& frame = item2.first;
			auto& hookName = item2.second;
			
			aniManager->setFrameEvent(animationName, frame, hookName);

		}
	}







	

	// set hook event at each frame's userInfo.
	/*
	for (auto& item : _mcFrameEvents) {
		auto spriteFrame = animationWalkFrames.at(item.first);
		auto userInfo = spriteFrame->getUserInfo();
		userInfo["hook_name"] = item.second;
		animationWalkFrames.at(item.first)->setUserInfo(userInfo);
	}
	*/
	// set event listener and dispatch to hookactionmanager to be invoked.
	/*
	auto listenerAnimationFrameDisplayed = EventListenerCustom::create(AnimationFrameDisplayedNotification, [](EventCustom* event) {
		auto info = static_cast<AnimationFrame::DisplayedEventInfo*>(event->getUserData());
		auto hookActionManager = apHookActionManager::getInstance();
		if (info->userInfo->find("hook_name") != info->userInfo->end()) {
			const auto& hook_name = info->userInfo->at("hook_name").asString();
			hookActionManager->runHook(hook_name);
		}
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listenerAnimationFrameDisplayed, 1);
	*/
	auto hookActionManager = apHookActionManager::getInstance();
	hookActionManager->addAction("power_frame_event", "gg", [this]() {
		_debugBox->get() << "frame 5 reached!!!" << DebugBox::push;
	});
	
	_sp = aniManager->createSprite("walk");
	//runLocalAction(_sp, RepeatForever::create(Animate::create(animationWalk)));
	//runLocalAction(_sp, Animate::create(animationWalk));
	//_sp->runAction(RepeatForever::create(Animate::create(animationWalk)));
	_sp->setScale(0.375f);
	//_sp->setScaleX(_boxWidth / _sp->getContentSize().width);
	//_sp->setScaleY(_boxHeight / _sp->getContentSize().height);
	//_masterField->addChild(_sp);
	auto moveRight = MoveBy::create(1, Vec2(50, 0));
	runLocalAction(_sp, moveRight);

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

	
	auto& startCoord = _maps["level"][_level.c_str()]["start"];
	b2BodyDef charBodyDef;
	//charBodyDef.position.Set((startCoord["x"].GetInt() * OneBlockPx) / SCALE_RATIO, (startCoord["y"].GetInt() * OneBlockPx) / SCALE_RATIO);
	//charBodyDef.userData = _sp;
	charBodyDef.fixedRotation = true;
	charBodyDef.type = b2BodyType::b2_dynamicBody;

	_charBody = world->CreateBody(&charBodyDef);
	_charBody->CreateFixture(&charFixtureDef);

	_characterNode = STBox2dNode::createWithBox2dBody(_charBody);
	_characterNode->addChild(_sp);
	_masterField->addChild(_characterNode);
	_characterNode->setBodyPositionBlock(Vec2(startCoord["x"].GetInt(), startCoord["y"].GetInt()));
	_charBody->SetUserData(_characterNode);

	// origin Air resistance setting.
	
	_originAirResistance = _airResistance;


	//----------------------------------
	// Character ContactListener Collision Callback
	//----------------------------------
	enum ContactDirection {
		C_LEFT,
		C_RIGHT,
		C_UP,
		C_DOWN
	};


	// if the character moving right and hit the wall of left side, then normal direction is point "left"
	auto isNormalDirection = [](const b2WorldManifold& worldManifold, ContactDirection direction)->bool {
		auto similar = [](float value, int a)->bool {
			switch (a) {
			case 1:
				return value > 0.999f;
			case 0:
				return value < 0.001f && value > -0.001f;
			case -1:
				return value < -0.999f;
			default:
				break;
			}
			return false;
		};
		
		switch (direction) {
		case C_LEFT:
			return
				similar(worldManifold.normal.x, -1) &&
				similar(worldManifold.normal.y, 0);
		case C_RIGHT:
			return
				similar(worldManifold.normal.x, 1) &&
				similar(worldManifold.normal.y, 0);

		case C_UP:
			return
				similar(worldManifold.normal.x, 0) &&
				similar(worldManifold.normal.y, 1);

		case C_DOWN:
			return
				similar(worldManifold.normal.x, 0) &&
				similar(worldManifold.normal.y, -1);

		default:
			break;
		}
		return false;
	};

	


	_cl_p->setBeginContact(_charBody, [this, isNormalDirection](b2Contact* contact, b2Fixture* other) {
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		cocos2d::log("begContact normalX:%.2f, normalY:%.2f", worldManifold.normal.x,
			worldManifold.normal.y);
		if (isNormalDirection(worldManifold, C_UP)) {

			//if the first floor from high air the player was.
			if (_floorFixtures.empty()) {
				_goFloorEffect = true;
			}
			_floorFixtures.emplace(other);
			
		}
	

		/*// if hit Ground!
		if (impulse->normalImpulses[0] >= 0.05f &&
			worldManifold.normal.y == 1.0f &&
			worldManifold.normal.x == 0.0f) {



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
		}*/

	});


	_cl_p->setPostSolve(_charBody,
		[this, isNormalDirection](b2Contact* contact, const b2ContactImpulse* impulse, b2Fixture* other) {

		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		auto actionManager = apHookActionManager::getInstance();

		_hitPower = impulse->normalImpulses[0];
		if (_hitPower >= 0.05f) {
			
		}
		/*cocos2d::log("nX:%.2f, nY:%.2f, inp0:%.2f, inp1:%.2f, itp0:%.2f, itp1:%.2f",
			worldManifold.normal.x,
			worldManifold.normal.y,
			impulse->normalImpulses[0],
			impulse->normalImpulses[1], // useless. -107374176.00
			impulse->tangentImpulses[0], // useless. -0.00
			impulse->tangentImpulses[1]); // useless. -107374176.00*/
		// if hit Ground!
		if (_goFloorEffect == true
			/*impulse->normalImpulses[0] >= 0.05f*/ &&
			isNormalDirection(worldManifold, C_UP)) {

			
				
			// power max : 0.2f
			float power = impulse->normalImpulses[0];
			if (power >= 0.2f) {
				power = 0.2f;
			}
			actionManager->runHook("character_hit_ground", power);
			_goFloorEffect = false;
			//characterHitGround(power);



		}
		// hit walls if character is moving left
		else if (/*(impulse->normalImpulses[0] >= 0.01f ||
			impulse->normalImpulses[1] >= 0.01f) &&*/
			isNormalDirection(worldManifold, C_RIGHT)) {

			if (_isDashing == true) {
				actionManager->runHook("character_dash_wall_left", impulse->normalImpulses[0]);
				actionManager->runHook("dash_end");
			}

			//characterHitLeftWall(impulse->normalImpulses[0]);
			//_characterHitLeftWallYes = true;
		}

		// hit walls if character is moving right
		else if (/*(impulse->normalImpulses[0] >= 0.01f ||
			impulse->normalImpulses[1] >= 0.01f) &&*/
			isNormalDirection(worldManifold, C_LEFT)) {

			if (_isDashing == true) {
				actionManager->runHook("character_dash_wall_right", impulse->normalImpulses[0]);
				actionManager->runHook("dash_end");
			}
			//characterHitRightWall(impulse->normalImpulses[0]);
			//_characterHitRightWallYes = true;
		}

	});

	_cl_p->setEndContact(_charBody, [this](b2Contact* contact, b2Fixture* other) {
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		cocos2d::log("endContact normalX:%.2f, normalY:%.2f", worldManifold.normal.x,
			worldManifold.normal.y);

		//remove fixture from floor fixtures set.
		if (_floorFixtures.find(other) != _floorFixtures.end()){
			_floorFixtures.erase(other);
		}
	});


	// set status if the character Hits Ground or Wall.
	/*_localUpdater->addFunc(P_WALLCHECK, [this](float)->void {
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
	
	});*/


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
				auto node = (Node*)body->GetUserData();
				node->setPosition(
					Vec2(body->GetPosition().x * SCALE_RATIO,
						body->GetPosition().y * SCALE_RATIO));
				node->setRotation(-1 * CC_RADIANS_TO_DEGREES(body->GetAngle()));

			}
		//clear forces 
		world->ClearForces();
	});

	// camera
	_localUpdater->addFunc(P_CAMERA, [this](float delta) {
		// camera update with vibration offset. 4
		auto camPos = _camera->getCameraPosition();
		//_camera->setVibrationOffset(_vibrationOffset);
		auto posToMove = _characterNode->getPosition();
		_camera->setCameraPosition(camPos + (posToMove - camPos) * _cameraMoveSpeed);
		//_camera->updateCamera(delta);


		auto size = Director::getInstance()->getVisibleSize();
		for (auto& item : _backgroundFollowRatio) {
			auto& backgroundNode = item.first;
			auto& followingRatio = item.second;
			auto d = _masterField->getPosition().x * followingRatio;
			while (d < -size.width) {
				d += size.width;
			}
			backgroundNode->setPosition(d, 0);
		}
		//_debugBox->get() << "x:" << _masterField->getPosition().x << " y:" << _masterField->getPosition().y << _debugBox->push;
		//make background move.
	});


	//drawnode polygon test 
	auto dn = DrawNode::create();
	_masterField->addChild(dn);
	dn->setPosition(Vec2(300, 300));
	Vec2 verts[5] = { {0,100},{100,100}, {50,50}, {100,0},{0,50} };
	dn->drawPolygon(verts, 5, Color4F(1.f, 1.f, 1.f, 1.f), 0, Color4F(0, 0, 0, 0));


	
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
std::shared_ptr<STContactListener> IngameScene::getContactListener()
{
	return IngameScene::_cl_p;
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
/*
void IngameScene::characterHitGround(float power) {
	_camera->vibrateCameraDirection(
		power * _cameraVRatio,
		power * _cameraVDuration,
		0.5f * M_PI);

	cocos2d::log("hit the ground power is %f", power);
	boost::coroutines::symmetric_coroutine<void>::call_type gogo(
		[this](boost::coroutines::symmetric_coroutine<void>::yield_type& yield) {
		_effectGen->generateEffect(_effectConf, _characterNode->getPosition().x, _characterNode->getPosition().y - _boxHeight / 2);
	});
	apAsyncTaskManager::getInstance()->addTask(std::move(gogo));
	
	// Sound Test
	auto ae = CocosDenshion::SimpleAudioEngine::getInstance();
	//ae->preloadEffect("sounds/stone2.ogg");
	//ae->playEffect("sounds/stone2.ogg", false);
	//ae->playEffect("sounds/stone.mp3");
	//ae->playEffect("sounds/stone.wav");

}*/

/*void IngameScene::characterHitLeftWall(float power) {
	cocos2d::log("leftWallHitPower : %f", power);
	_debugBox->get() << power << DebugBox::push;
	// wall bouncing
	if (_isDashing == true) {
		
		
	}
	apHookActionManager::getInstance()->runHook("dash_end");
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
	apHookActionManager::getInstance()->runHook("dash_end");
}*/


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

STCamera * IngameScene::getCamera()
{
	return _camera;
}

Vec2 IngameScene::getMasterFieldPosition()
{
	return _masterField->getPosition();
}

Node * IngameScene::getMasterField()
{
	return _masterField;
}

DebugBox * IngameScene::getDebugBox()
{
	return _debugBox;
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

std::shared_ptr<IngameScene::UpdateCaller> IngameScene::getLocalUpdater()
{
	return _localUpdater;
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
/*
cocos2d::Vector<SpriteFrame*> IngameScene::Util::getAnimationFrame(const char * format, int start, int end)
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
}

cocos2d::Vector<SpriteFrame*> IngameScene::Util::getAnimationFrame(const char * format, const FrameRange & frameRange)
{
	return getAnimationFrame(format, frameRange.start, frameRange.end);
}

cocos2d::Animation * IngameScene::Util::getAnimation(const char * format, const FrameRange & frameRange)
{
	auto frames = getAnimationFrame(format, frameRange);
	auto animation = Animation::createWithSpriteFrames(frames, 1 / 15.f, 1);
	return animation;
}
*/
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


