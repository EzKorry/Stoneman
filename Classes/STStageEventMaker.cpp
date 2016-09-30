#include "STStageEventMaker.h"
#include "IngameScene.h"
#include "DebugBox.h"
#include "STWall.h"
#include "STTextBoxNode.h"
#include "apTouchManager.h"
#include "STCamera.h"

void STStageEventMaker::makeStageEvent(const string & level)
{
	if (level == "1") makeLevel1();
	else if (level == "tutorial") makeTutorial();
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
	cl->setBeginContact(charBody, [scene, charBody](b2Contact* contact, b2Fixture* other) {
		auto wall = scene->getWallByBody(other->GetBody());
		scene->getDebugBox()->get() << wall->getName() << arphomod::DebugBox::push;

		if (wall->getName() == "power") {
			auto b2v = charBody->GetPosition();
			auto v = Vec2(b2v.x *SCALE_RATIO, b2v.y * SCALE_RATIO + 30);
			scene->getTextBoxNode()->createTextBox(scene->getText("power_hit"), v);
			scene->getTextBoxNode()->createFollowingTextBox("poswr vkdji xjfi dksfjkdf kdfjskdf sdk d sjfk sjsk", charBody, Vec2(0.f, 30.f));



		}
	});
}

void STStageEventMaker::makeTutorial()
{
	auto scene = IngameScene::getInstance();
	auto cl = scene->getContactListener();
	auto charBody = scene->getCharBody();
	auto am = apHookActionManager::getInstance();
	auto tm = APTouchManager::getInstance();

	auto dm = apDataManager::getInstance();
	dm->set("tutorial_1", false);

	auto setEnableClickNodes = [](bool enabled) {
		auto tm = APTouchManager::getInstance();
		auto camera = IngameScene::getInstance()->getCamera();
		vector<string> nodes = { "rightButton", "leftButton", "upButton", "skillButton" };
		for (auto& node : nodes) {
			tm->setEnabledNode(camera->getChildByName(node), enabled);
		}
	};

	auto hideSkillAndUpButton = []() {
		auto cam = IngameScene::getInstance()->getCamera();
		auto skill = cam->getChildByName("skillButton");
		auto up = cam->getChildByName("upButton");
		if (skill && up) {
			skill->setOpacity(0);
			up->setOpacity(0);
		}
	};

	auto showSlowlyButton = [scene](cocos2d::Sprite* sp) {

		auto appear = FadeTo::create(1.f, 255);
		auto ease = EaseInOut::create(appear, 3.f);

		scene->runLocalAction(sp, appear);
	};

	cl->setBeginContact(charBody, [scene, charBody, setEnableClickNodes](b2Contact* contact, b2Fixture* other) {
		auto wall = scene->getWallByBody(other->GetBody());
		auto dm = apDataManager::getInstance();
		auto& settings = scene->getSettings();
		auto ballUpColor = Color3B(settings["colors"]["tutorial_ball_up"]["r"].GetInt(),
			settings["colors"]["tutorial_ball_up"]["g"].GetInt(),
			settings["colors"]["tutorial_ball_up"]["b"].GetInt());
		auto ballDownColor = Color3B(settings["colors"]["tutorial_ball_down"]["r"].GetInt(),
			settings["colors"]["tutorial_ball_down"]["g"].GetInt(),
			settings["colors"]["tutorial_ball_down"]["b"].GetInt());

		GLubyte ballUpOpacity = settings["colors"]["tutorial_ball_up"]["a"].GetInt();
		auto skillPosition = scene->getCamera()->getChildByName("skillButton")->getPosition();
		auto upPosition = scene->getCamera()->getChildByName("upButton")->getPosition();
		auto leftPosition = scene->getCamera()->getChildByName("leftButton")->getPosition();
		auto rightPosition = scene->getCamera()->getChildByName("rightButton")->getPosition();


		auto changeBallState = [ballDownColor, ballUpColor, ballUpOpacity](cocos2d::Node* ball, bool pushed) {
			if (pushed) {
				ball->setColor(ballDownColor);
				ball->setOpacity(255);
			}
			else {
				ball->setColor(ballUpColor);
				ball->setOpacity(ballUpOpacity);
			}
		};

		/****************************/
		/*************not1***********/
		/****************************/

		if (dm->get<bool>("tutorial_1") == false &&
			wall->getName() == "not1") {

			dm->set("tutorial_1", true);

			setEnableClickNodes(false);
			auto tm = APTouchManager::getInstance();
			tm->cancelAllTouch();
			scene->cancelAllInput();
			

			auto visibleSize = Director::getInstance()->getVisibleSize();
			auto ballSpriteFrameName = scene->getSettings()["sprite_frames"]["tutorial_ball"].GetString();
			auto ball = Sprite::createWithSpriteFrameName(ballSpriteFrameName);
			ball->setPosition(visibleSize.width / 2 + 30.f, visibleSize.height / 2);
			ball->setCascadeOpacityEnabled(true);
			ball->setOpacity(false);
			ball->setCascadeColorEnabled(true);
			ball->setColor(ballUpColor);
			ball->setScale(50.f / ball->getContentSize().width);
			scene->getCamera()->addChild(ball, 999, "tutorial_ball_1");

			auto ball2 = Sprite::createWithSpriteFrameName(ballSpriteFrameName);
			ball2->setPosition(visibleSize.width / 2 - 30.f, visibleSize.height / 2);
			ball2->setCascadeOpacityEnabled(true);
			ball2->setOpacity(false);
			ball2->setCascadeColorEnabled(true);
			ball2->setColor(ballUpColor);
			ball2->setScale(50.f / ball2->getContentSize().width);
			scene->getCamera()->addChild(ball2, 999, "tutorial_ball_2");
			
			//----------------

			auto emerge = FadeTo::create(1.f, ballUpOpacity);
			auto emergeEase = EaseInOut::create(emerge, 2.5f);
			auto emergeUp = CallFunc::create([scene]() {
				auto skill = scene->getCamera()->getChildByName("upButton");
				auto skillOpacity = FadeTo::create(0.5f, 255);
				auto ease = EaseInOut::create(skillOpacity, 3.f);
				scene->runLocalAction(skill, ease);
			});
			auto delay1_1 = DelayTime::create(0.5f);
			auto move1_1 = MoveTo::create(1.3f, upPosition);
			auto move1_1Ease = EaseExponentialInOut::create(move1_1);
			auto delay1_2 = DelayTime::create(1.2f);
			auto pf1 = CallFuncN::create([changeBallState](cocos2d::Node* node) {
				auto apm = apHookActionManager::getInstance();
				apm->runHook("upDown");
				changeBallState(node, true);
			});
			auto delay1_3 = DelayTime::create(0.5f);
			auto pf2 = CallFuncN::create([changeBallState](cocos2d::Node* node) {
				auto apm = apHookActionManager::getInstance();
				apm->runHook("upEndedIn");
				changeBallState(node, false);
			});
			auto delay1_4 = DelayTime::create(1.f);
			auto hide = FadeTo::create(1.f, 0);
			auto hideEase = EaseInOut::create(hide, 3.f);
			auto setEnableTrue = CallFunc::create([setEnableClickNodes,scene]() {
				auto tm = APTouchManager::getInstance();
				setEnableClickNodes(true);
				tm->setEnabledNode(scene->getCamera()->getChildByName("skillButton"), false);
			});
			auto destroy = CallFuncN::create([](cocos2d::Node* node) {
				node->removeFromParent();
			});

			
			//----------------

			auto move2_1 = MoveTo::create(1.3f, rightPosition);
			auto move2_1Ease = EaseExponentialInOut::create(move2_1);
			auto delay2_2 = DelayTime::create(0.5f);
			auto pf2_1 = CallFuncN::create([changeBallState](cocos2d::Node* node) {
				auto apm = apHookActionManager::getInstance();
				apm->runHook("rightDown");
				changeBallState(node, true);
			});
			auto delay2_3 = DelayTime::create(1.2f);
			auto pf2_2 = CallFuncN::create([changeBallState](cocos2d::Node* node) {
				auto apm = apHookActionManager::getInstance();
				apm->runHook("rightEndedIn");
				changeBallState(node, false);
			});

			//----------------

			auto seq1 = Sequence::create(
				emergeEase->clone(), 
				emergeUp, 
				delay1_1->clone(), 
				move1_1Ease, 
				delay1_2, 
				pf1, 
				delay1_3, 
				pf2, 
				delay1_4->clone(), 
				hideEase->clone(),
				setEnableTrue, 
				destroy->clone(), 
				nullptr);
			scene->runLocalAction(ball, seq1);

			auto seq2 = Sequence::create(
				emergeEase,
				delay1_1,
				move2_1Ease,
				delay2_2, 
				pf2_1, 
				delay2_3,
				pf2_2,
				delay1_4, 
				hideEase,
				destroy, 
				nullptr);
			scene->runLocalAction(ball2, seq2);


		}
	});

	apHookActionManager::getInstance()->addAction("after_display_stage", "event", IngameScene::getInstance()->getCamera(), 
		[hideSkillAndUpButton,setEnableClickNodes, scene](const char* level) {
		if (string(level) == "tutorial") {
			hideSkillAndUpButton();
			auto tm = APTouchManager::getInstance();
			tm->setEnabledNode(scene->getCamera()->getChildByName("upButton"), false);
			tm->setEnabledNode(scene->getCamera()->getChildByName("skillButton"), false);
			

			//auto actions = Sequence::create();
			//scene->runLocalAction(scene->getCamera(), actions);

		}
	});

//	auto dm = apDataManager::getInstance();
//	dm->set("entered_tutorial", true);


}
