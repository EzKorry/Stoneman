/*
 * IngameScene.h
 *
 *  Created on: 2015. 10. 15.
 *      Author: ������
 */

#ifndef INGAMESCENE_H_
#define INGAMESCENE_H_


#include "STMacros.h"
#include <cocos2d.h>
#include "json/rapidjson.h"
#include "json/document.h"
#include "Box2D/Box2D.h"
#include "ui/CocosGUI.h"
#include "STContactListener.h"
#include "STEffectGenerator.h"
USING_NS_CC;
using namespace cocos2d::ui;
using namespace arphomod;


// character width:30px, height:48px.



enum EntityCategory {
	WALL = 0x0001,
	CHARACTER = 0x0002,
	BOSS = 0x0004,
	EFFECT_PARTICLE = 0x0008,
	ENEMY_AIRCRAFT = 0x0010,
	LOCATION = 0x0020
};

enum EffectType {
	ROCK_LITTLE_CRACK,
	ROCK_LITTLE_CRACK_FLOOR,
	DUST,
	DUST_FLOOR
};

enum ScheduleUpdateFuncPriority {
	P_WORLDSTEP = 1,
	P_AFTER_WORLDSTEP = 2,
	P_CAMERA = 5,
	P_REPLACEMAPSPRITES = 6,
	P_FOLLOWNODE = 7
};

enum MasterFieldZOrder {
	MFZORDER_CHARNODE = 9,
	MFZORDER_WALLBUILDER = 10,
	MFZORDER_EFFGEN = 11,
	MFZORDER_TEXTBOX = 12,
	MFZORDER_EMPH_BLACK = 14,
	MFZORDER_EMPH_CHARNODE = 15
};

class STCamera;
class GLESDebugDraw;
class STWallBuilder;
class STBox2dNode;
class STWall;
class STTextBoxNode;

namespace arphomod {
	class DebugBox;
	struct FrameRange;
}
class IngameScene : public cocos2d::Scene {
private:
	class Util {
	public:

		// number to string
		template<typename T>
		std::string toString(T Number)
		{
			std::ostringstream ss;
			ss << Number;
			return ss.str();
		}
		
		// return b as a's positioning.
		// ex) aStart~aEnd : 0~10, bStart~bEnd:0~100, if a==7.5 then return 75.
		float bySameRatio(float aStart, float a, float aEnd, float bStart, float bEnd);

		// adjust scale x or y, fitting one side width or height.
		// 300x500 image, side=200, isWidth=true then 200x333 image created.
		void resizeSprite(cocos2d::Sprite* sprite, float side, bool sideIsWidth);
		
		// t : current time
		// b : start value
		// c : change in value b:100, c:-300, if 100%: -200(c+b)
		// d : duration.
		// http://www.gizma.com/easing/
		inline float easeInExpo(float t, float b, float c, float d) {
			return c * std::pow(2, 10 * (t / d - 1)) + b;
		};
		
		Color4F switchColor(const Color4B& color) {
			return Color4F(color);
		}
		/*

		// get Sprite Frames to use implementing animation.
		// it should be called after spritecache->addSpriteFramesWithFile("something.plist");
		cocos2d::Vector<SpriteFrame*> getAnimationFrame(const char *format, int start, int end);

		// get Sprite Frames to use implementing animation.
		// it should be called after spritecache->addSpriteFramesWithFile("something.plist");
		cocos2d::Vector<SpriteFrame*> getAnimationFrame(const char *format, const FrameRange& frameRange);

		// get Sprite Frames to use implementing animation.
		// it should be called after spritecache->addSpriteFramesWithFile("something.plist");
		cocos2d::Animation* getAnimation(const char *format, const FrameRange& frameRange);
		*/

		Util();
		~Util();
	};

	// call functions based priority when main scene updating 
	class UpdateCaller {
	public:

		void initWithScheduler(cocos2d::Scheduler* s, const std::string& key);

		// lower number, higher priority.
		// ex) 1:first, 10:last.
		// priority cannot be overwritten.
		template<typename TFunc>
		void addFunc(int priority, TFunc&& func){
			if (_list.find(priority) != _list.end()) {
				cocos2d::log("UpdateCaller - already exists!");
				return;
			}
			
			_list[priority] = std::forward<TFunc>(func);

		}

		template<typename TFunc>
		void changeFunc(int priority, TFunc&& func) {
			if (_list.find(priority) != _list.end()) {
				_list[priority] = std::forward<TFunc>(func);
				return;
			}
			cocos2d::log("UpdateCaller-changeFunc - key doesn't exist!");
		}

		

		void delFunc(int priority);

		void delAllFunc();
		UpdateCaller();
		~UpdateCaller();
	private:
		std::map<int, std::function<void(float)>> _list;
	};


public:
	IngameScene();
	virtual ~IngameScene();
	CREATE_FUNC(IngameScene);
	bool init() override;

	// Singleton method
	static IngameScene* getInstance();
	
	// method called every frame.
	void update(float delta) override;

	Util util{};
	void draw(Renderer *renderer, const Mat4& transform, uint32_t transformUpdated) override;
	// per frame.
	
	void addWall(float px, float py, float w, float h);
	void setJumpPower(float power);
	void addTextField(const std::function<void(cocos2d::ui::EditBox*)>& callback, const std::string& placeHolder);
	void addTextField(float* fp, const std::string& placeHolder);
	void replaceBox();

	//b2world pointer getter
	static std::shared_ptr<b2World> getb2World();
	static std::shared_ptr<STContactListener> getContactListener();
	static const float OneBlockPx;
	static float _timeScale;

	//get Camera
	STCamera* getCamera();

	// get masterField's position.
	Vec2 getMasterFieldPosition();
	Node* getMasterField();

	//Get Wall By Name
	STWall* getWallByBody(b2Body* body);

	//get debug Box
	DebugBox* getDebugBox();

	//get character's box2d body.
	b2Body* getCharBody();

	//get TextBoxNode
	STTextBoxNode* getTextBoxNode();

	//get Korean Text from json.
	string getText(const string& key);

	//get Settings
	rapidjson::Document& getSettings();

	// local Scheduler methods.
	Scheduler* getLocalScheduler() { return _localScheduler; }
	void scheduleLocally(const std::function<void(float)>& callback, cocos2d::Node* target, float interval, unsigned int repeat, float delay, const std::string &key);
	void scheduleLocally(const std::function<void(float)>& callback, cocos2d::Node * target, const std::string & key);
	void unscheduleLocally(const std::string &key);
	void scheduleOnceLocally(const std::function<void(float)> &callback, float delay, const std::string &key);
	Action * runLocalAction(Node* target, Action* action);
	
	//getter
	std::shared_ptr<UpdateCaller> getLocalUpdater();

	//cancel all touching.
	void cancelAllInput();




private:

	static IngameScene* _uniqueScene;

	//when init, you should call debug variable!
	void appInit();
	void organizeScene();
	void debugVariable();
	void animationTest();
	void gameInterface();
	void initializePhysics(const std::string& level);
	void initializeEffectManager();

	
	// jump touch end callback.
	void jumpTouchEnd();

	//void characterHitGround(float power);
	//void characterHitLeftWall(float power);
	//void characterHitRightWall(float power);

	// box2d stepping parameters.
	int _positionIterations{ 3 };
	int _velocityIterations{ 3 };

	
	// box2d world
	static std::shared_ptr<b2World> _world;

	float _jumpPower{ 2.0f };
	float _airResistance{ 0.02f }, _originAirResistance{ 0.0f }, _maxAirResistance{ 1.5f }, _nowMaxAirResistance{ 0.0f };
	float _movePower{ 10.0f };

	
	// box Width and height are going to be initialized by appInit();
	float _boxWidth{ 0.0f }, _boxHeight{ 0.0f }, _boxDensity{ 0.01f };
	float _cameraMoveSpeed{ 0.3f };
	float _dashPower{ 8.0f }, _dashAirRatio{ 0.1f }, _dashAngle{ 0.06f * (float)M_PI}, _dashDuration{ 0.3f };
	bool _isDashing = false;
	float _gravityScaleWhenJump{ 0.3f };

	// camera viberation.
	float _cameraVRatio{ 50.0f };
	float _cameraVDuration{ 5.0f };
	const float _dashWallCondition{ 0.2f };

	//how long slow motion
	float _slowMotionDuration{ 0.3f };
	int _slowMotionSpOpacity{ 70 };
	// for left wall hit
	float _dashWallBounceRadian{ 0.05f * (float)M_PI };
	float _dashWallBouncePower{ 6.0f };
	bool _characterHitGroundYes{ false };
	bool _characterHitLeftWallYes{ false };
	bool _characterHitRightWallYes{ false };
	float _hitPower;
	std::set<b2Fixture*> _floorFixtures;

	//default Background color.
	std::map<string, Color4B> _colors;

	//debugBox
	DebugBox* _debugBox;

	STEffectGenerator* _effectGen;
	std::map<EffectType, STEffectConfigure> _effectConfs;
	STWallBuilder* _wallBuilder;
	
	// localScheduler for slow motion.
	// It affects by "Pause", or slow motion.
	static Scheduler* _localScheduler;
	static ActionManager* _localActionManager;

	// updater.
	std::shared_ptr<UpdateCaller> _localUpdater, _globalUpdater;

	// json
	rapidjson::Document _maps;
	rapidjson::Document _settings;

	// level Name
	std::string _level{ "" };

	// path string store map
	std::vector<string> _plistPath;

	std::map<string, string> _spriteFrames;

	// background node Map
	// ratio: 1 = absolutely like masterfield, 0 = stick at camera.
	std::map<cocos2d::Node*,float> _backgroundFollowRatio;

	// background node
	cocos2d::Node* _backgroundField;
	
	// true if the player lasts in the air after that he was highest.
	bool _checkJumpHighest{ false };

	// true if it has to make floor effect.
	// it sets true when contact listener begin.
	// it sets false right after hook action executed.
	bool _goFloorEffect{ false };

	// used for debug box
	cocos2d::Vec2 _nowTextPos { Vec2(50.0f, 15.0f) };
	cocos2d::Vec2 _vibrationOffset {Vec2::ZERO };

	// main character sprite and animation.
	cocos2d::Sprite* _sp { nullptr };
	std::unordered_map<std::string, FrameRange> _mcFrameRanges;
	std::unordered_map<std::string, std::unordered_map< int, std::string >> _mcFrameEvents;
	std::string _mcFramePath;

	// box2d main character body
	b2Body* _charBody{ nullptr };

	// main character node. It contains character body and animation.
	STBox2dNode* _characterNode{ nullptr };

	cocos2d::Node* _masterField { nullptr };
	STCamera* _camera { nullptr };

	// textBox ex) character saying
	STTextBoxNode* _textBoxNode{ nullptr };

	

	b2CircleShape ballShape;
	b2BodyDef ballBodyDef;
#if  (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::shared_ptr<GLESDebugDraw> _debugDraw { nullptr};
#endif

	//box2d contact listener.
	static std::shared_ptr<STContactListener> _cl_p;

	//GLESDebugDraw *_debugDraw { nullptr };

	bool _checkMoveButton { false };
	bool _isRight { false };

	// used when keyboard testing
	int _keyCount = 0;

	//cocos2d::Node* _box2dWorld { nullptr };

	
};

#endif /* INGAMESCENE_H_ */
