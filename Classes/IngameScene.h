/*
 * IngameScene.h
 *
 *  Created on: 2015. 10. 15.
 *      Author: ±Ë≈¬»∆
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
};

enum ScheduleUpdateFuncPriority {
	P_WORLDSTEP = 1,
	P_WALLCHECK = 3,
	P_CAMERA = 5
};

class STCamera;
class GLESDebugDraw;
class STWallBuilder;
class STBox2dNode;

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
	static std::shared_ptr<b2World> getb2World();
	static const float OneBlockPx;
	static float _timeScale;



	b2Body* getCharBody();
	std::vector<b2Body*> getWalls();

	// local Scheduler methods.
	Scheduler* getLocalScheduler() const { return _localScheduler; }
	void scheduleLocally(const std::function<void(float)>& callback, float interval, unsigned int repeat, float delay, const std::string &key);
	void unscheduleLocally(const std::string &key);
	void scheduleOnceLocally(const std::function<void(float)> &callback, float delay, const std::string &key);
	Action * runLocalAction(Node* target, Action* action);




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


	// character dashes.
	void doDash();

	// character finishes dashing.
	void endDash();

	
	// jump touch end callback.
	void jumpTouchEnd();

	void characterHitGround(float power);
	void characterHitLeftWall(float power);
	void characterHitRightWall(float power);

	// box2d stepping parameters.
	int _positionIterations{ 3 };
	int _velocityIterations{ 3 };

	
	// box2d world
	static std::shared_ptr<b2World> world;

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

	STEffectConfigure _effectConf;
	STEffectGenerator* _effectGen;
	STWallBuilder* _wallBuilder;
	
	// localScheduler for slow motion.
	// It affects by "Pause", or slow motion.
	Scheduler* _localScheduler{ nullptr };
	ActionManager* _localActionManager{ nullptr };

	// updater.
	std::shared_ptr<UpdateCaller> _localUpdater, _globalUpdater;

	// json
	rapidjson::Document _maps;

	// level Name
	std::string _level{ "" };

	// path string store map
	std::map<string, string> _path;

	// background node Map
	// ratio: 1 = absolutely like masterfield, 0 = stick at camera.
	std::map<cocos2d::Node*,float> _backgroundFollowRatio;

	// background node
	cocos2d::Node* _backgroundField;
	
	bool _checkJumpHighest{ false };
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

	

	b2CircleShape ballShape;
	b2BodyDef ballBodyDef;
#if  (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::shared_ptr<GLESDebugDraw> _debugDraw { nullptr};
#endif
	std::shared_ptr<STContactListener> _cl_p { nullptr};
	//GLESDebugDraw *_debugDraw { nullptr };
	std::vector<b2Body*> _walls;
	bool _checkMoveButton { false };
	bool _isRight { false };
	bool _haveToGenerateEffect{ false };
	// used when keyboard testing
	int _keyCount = 0;

	cocos2d::Node* _box2dWorld { nullptr };

	
};

#endif /* INGAMESCENE_H_ */
