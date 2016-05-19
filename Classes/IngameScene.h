/*
 * IngameScene.h
 *
 *  Created on: 2015. 10. 15.
 *      Author: ±Ë≈¬»∆
 */

#ifndef INGAMESCENE_H_
#define INGAMESCENE_H_

#define SCALE_RATIO 32.0
#define M_PI 3.14159265358979323846

#include <cocos2d.h>
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


class STCamera;
class GLESDebugDraw;
class STWallBuilder;
namespace arphomod {
	class DebugBox;
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

		Util();
		~Util();
	};
public:
	IngameScene();
	virtual ~IngameScene();
	CREATE_FUNC(IngameScene);
	bool init() override;
	

	Util util{};
	void draw(Renderer *renderer, const Mat4& transform, uint32_t transformUpdated) override;
	// per frame.
	void update(float delta) override;
	void addWall(float px, float py, float w, float h);
	void setJumpPower(float power);
	void jumpClicked();
	void addTextField(const std::function<void(cocos2d::ui::EditBox*)>& callback, const std::string& placeHolder);
	void addTextField(float* fp, const std::string& placeHolder);
	void replaceBox();
	static std::shared_ptr<b2World> getb2World();
	static const float OneBlockPx;
	static float _timeScale;

	
	static IngameScene* getInstance();

	b2Body* getCharBody();
	std::vector<b2Body*> getWalls();

	Scheduler* getLocalScheduler() const { return _localScheduler; }
	Action * runLocalAction(Node* target, Action* action);



private:

	static IngameScene* _uniqueScene;

	//when init, you should debug variable!
	void debugVariable();
	void animationTest();
	void gameInterface();
	void initializePhysics();
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

	float powerMultiplier{ 0.005f };
	float _jumpPower{ 2.0f };
	float _airResistance{ 0.02f }, _originAirResistance{ 0.0f }, _maxAirResistance{ 1.5f }, _nowMaxAirResistance{ 0.0f };
	float _movePower{ 10.0f };
	float _boxWidth{ 40.0f }, _boxHeight{ 64.0f }, _boxDensity{ 0.01f };
	float _cameraMoveSpeed{ 0.3f };
	float _dashPower{ 8.0f }, _dashAirRatio{ 0.1f }, _dashAngle{ 0.06f * (float)M_PI}, _dashDuration{ 0.3f };
	bool _isDashing = false;
	float _gravityScaleWhenJump{ 0.3f };
	float _cameraVRatio{ 50.0f };
	float _cameraVDuration{ 5.0f };
	int _jumpCount = 0;
	const float _dashWallCondition{ 0.2f };
	// for left wall hit
	float _dashWallBounceRadian{ 0.05f * (float)M_PI };
	float _dashWallBouncePower{ 6.0f };
	bool _characterHitGroundYes{ false };
	bool _characterHitLeftWallYes{ false };
	bool _characterHitRightWallYes{ false };
	float _hitPower;

	//debugBox
	DebugBox* _debugBox;

	STEffectConfigure _effectConf;
	STEffectGenerator* _effectGen;
	STWallBuilder* _wallBuilder;
	
	// localScheduler for slow motion.
	Scheduler* _localScheduler{ nullptr };
	ActionManager* _localActionManager{ nullptr };

	
	bool _checkJumpHighest{ false };
	cocos2d::Vec2 _nowTextPos { Vec2(50.0f, 15.0f) };
	cocos2d::Vec2 _vibrationOffset {Vec2::ZERO };
	cocos2d::Sprite* _sp { nullptr };
	cocos2d::Node* _masterField { nullptr };
	STCamera* _camera { nullptr };
	b2Body* _charBody { nullptr };
	b2CircleShape ballShape;
	b2BodyDef ballBodyDef;
	std::shared_ptr<GLESDebugDraw> _debugDraw { nullptr};
	std::shared_ptr<STContactListener> _cl_p { nullptr};
	//GLESDebugDraw *_debugDraw { nullptr };
	std::vector<b2Body*> _walls;
	bool _checkMoveButton { false };
	bool _isRight { false };
	bool _haveToGenerateEffect{ false };
	std::future<void> fut;
	// used when keyboard testing
	int _keyCount = 0;

	cocos2d::Node* _box2dWorld { nullptr };

	
};

#endif /* INGAMESCENE_H_ */
