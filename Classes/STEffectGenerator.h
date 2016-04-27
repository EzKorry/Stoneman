#ifndef _STEFFECTGENERATOR_H_
#define _STEFFECTGENERATOR_H_
#include <cocos2d.h>
#include <string>

using namespace cocos2d;

class STParticle;
class b2Body;
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
	float minAlpha = 255.f;
	float maxAlpha = 255.f;

	bool fixedRotation = false;

	// min 0, max 255.
	GLubyte colorBrightnessRange = 30;
	STEffectType type = STEffectType::Boom;
};
class STEffectGenerator : public cocos2d::Node {

public:
	STEffectGenerator();
	virtual ~STEffectGenerator();
	void generateEffect(const STEffectConfigure& configure, const cocos2d::Vec2& point);
	void generateEffect(const STEffectConfigure& configure, const float& x, const float& y);
	void stopEffect(const std::string& name);
	bool init() override;

	CREATE_FUNC(STEffectGenerator);

private:
	cocos2d::Vector<STParticle*> _particles;
	std::vector<b2Body*> _particleBodies;

};


#endif