#ifndef _STEFFECTGENERATOR_H_
#define _STEFFECTGENERATOR_H_
#include <cocos2d.h>
#include <Box2D\Box2D.h>
#include <string>
#include <array>

using namespace cocos2d;

class b2Body;
enum class STEffectTexture {
	Square,
	Circle
};

enum class STEffectType {
	Boom
};


struct STEffectConfigure {
	// particles must have the same weight(10);

	int count = 10;
	float minPower = 1.f;
	float maxPower = 3.f;
	float minSize = 1.f;
	float maxSize = 10.f;
	bool hasPhysics = true;
	uint16 maskBits = 0x0000;
	float minGravityScale = 0.5f;
	float maxGravityScale = 0.8f;
	float minDuration = 0.5f;
	float maxDuration = 1.0f;
	float restitution = 0.5f;
	float friction = 0.3f;

	// positioning randomness
	float diffuseX = 40.f;
	float diffuseY = 10.f;

	cocos2d::Color3B color = cocos2d::Color3B(51,42,34);

	// 0~255
	float minAlpha = 255.f;

	// 0~255
	float maxAlpha = 255.f;

	bool fixedRotation = false;

	// min 0, max 255.
	GLubyte colorBrightnessRange = 30;
	STEffectType type = STEffectType::Boom;
	STEffectTexture tex = STEffectTexture::Square;
};
class STEffectGenerator : public cocos2d::Node {

public:
	STEffectGenerator();
	virtual ~STEffectGenerator();
	void generateEffect(const STEffectConfigure& configure, const cocos2d::Vec2& point);
	void generateEffect(const STEffectConfigure& configure, const float& x, const float& y);
	void stopEffect(const std::string& name);
	
	/*cocos2d::SpriteBatchNode* getMap(STEffectTexture t);*/
	bool init() override;

	CREATE_FUNC(STEffectGenerator);
	const static size_t endIndex = 500;

private:
	
	//std::map<STEffectTexture, std::string> _textureFileName;
	cocos2d::Sprite* getNextSprite();
	std::map<STEffectTexture, std::string> _spriteFrameName;
	cocos2d::SpriteBatchNode* _batch;
	std::array<cocos2d::Sprite*, endIndex> _sprites{ nullptr };
	size_t index = 0;
	
};


#endif