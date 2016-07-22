#include "STEffectGenerator.h"
#include "IngameScene.h"
#include <vector>
#include <random>


STEffectGenerator::STEffectGenerator()
{
}

STEffectGenerator::~STEffectGenerator()
{
}

cocos2d::SpriteBatchNode * STEffectGenerator::getMap(STEffectTexture t)
{
	if (_sp[t] == nullptr) {
		std::string file;
		switch (t) {
		case STEffectTexture::Square:
			file = "white.png";
			break;
		default:
			file = "white.png";
			break;

		}
		_sp[t] = SpriteBatchNode::create(file);
		this->addChild(_sp[t]);
	}
	return _sp[t];
}

bool STEffectGenerator::init() {
	if (!Node::init()) return false;

	_textureFileName[STEffectTexture::Square] = "white.png";

	return true;
}

void STEffectGenerator::generateEffect(const STEffectConfigure & configure, const cocos2d::Vec2& point)
{
	
	// 값이 적법한지 검사.
	if (configure.minPower > configure.maxPower ||
		configure.minSize > configure.maxSize ||
		configure.minDuration > configure.maxDuration ||
		configure.minGravityScale > configure.maxGravityScale ||
		configure.minAlpha > configure.maxAlpha) {
		
		cocos2d::log("values are not valid!");
		return;
	}
	auto world = IngameScene::getb2World();
	if (world == nullptr) return;
	//vector<b2Body*> bodies;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float>
		d_power(configure.minPower, configure.maxPower),
		d_size(configure.minSize, configure.maxSize),
		d_duration(configure.minDuration, configure.maxDuration),
		d_gravityScale(configure.minGravityScale, configure.maxGravityScale),
		d_angle(0, M_PI * 2),
		d_diffuseX(-configure.diffuseX / 2.f, configure.diffuseX / 2.f),
		d_diffuseY(-configure.diffuseY / 2.f, configure.diffuseY / 2.f),
		d_alpha(configure.minAlpha, configure.maxAlpha),
		d_color(-configure.colorBrightnessRange, configure.colorBrightnessRange),
		d_powerPoint(-0.5f, 0.5f);

	int biggestG = 0;
	if (configure.color.r > biggestG) biggestG = configure.color.r;
	if (configure.color.g > biggestG) biggestG = configure.color.g;
	if (configure.color.b > biggestG) biggestG = configure.color.b;
	auto colorMaxTest = biggestG + configure.colorBrightnessRange;
	auto colorMinTest = biggestG - configure.colorBrightnessRange;
	GLubyte colorMax = 0;
	GLubyte colorMin = 255;
	if (colorMaxTest > 255) colorMax = 255;
	else colorMax = colorMaxTest;
	if (colorMinTest < 0) colorMin = 0;
	else colorMin = colorMinTest;

	bool isBlack = false;
	if (biggestG == 0) {
		isBlack = true;
		biggestG = 1;
	}
	float maxColorRatio = (float)colorMax / (float)biggestG;
	float minColorRatio = (float)colorMin / (float)biggestG;

	std::uniform_real_distribution<float>
		d_colorRatio(minColorRatio, maxColorRatio);




	for (int i = 0; i < configure.count; i++) {


		
		auto size = d_size(gen);
		auto power = d_power(gen);
		auto duration = d_duration(gen);
		auto gravityScale = d_gravityScale(gen);
		auto angle = d_angle(gen);
		auto diffuseX = d_diffuseX(gen);
		auto diffuseY = d_diffuseY(gen);
		auto colorRatio = d_colorRatio(gen);
		GLubyte r = (configure.color.r + (int)isBlack) * colorRatio;
		GLubyte g = (configure.color.g + (int)isBlack) * colorRatio;
		GLubyte b = (configure.color.b + (int)isBlack) * colorRatio;
		GLubyte a = d_alpha(gen);

		auto sp = Sprite::create(_textureFileName[configure.tex]);
		sp->setScale(size / 10);
		sp->setPosition(point.x + diffuseX, point.y + diffuseY);
		sp->setCascadeColorEnabled(true);
		sp->setColor(Color3B(r, g, b));
		sp->setOpacity(a);
		getMap(configure.tex)->addChild(sp);
		
		b2PolygonShape particleShape;
		particleShape.SetAsBox(size / SCALE_RATIO / 2, size / SCALE_RATIO / 2);

		b2FixtureDef particleFixtureDef;
		particleFixtureDef.density = 10.f / (size*size);
		particleFixtureDef.friction = configure.friction;
		particleFixtureDef.restitution = configure.restitution;
		particleFixtureDef.shape = &particleShape;
		particleFixtureDef.filter.categoryBits = EFFECT_PARTICLE;
		particleFixtureDef.filter.maskBits = WALL;

		b2BodyDef particleBodyDef;
		particleBodyDef.position.Set((point.x + diffuseX) / SCALE_RATIO, (point.y + diffuseY) / SCALE_RATIO);
		particleBodyDef.userData = sp;
		particleBodyDef.gravityScale = gravityScale;
		particleBodyDef.fixedRotation = configure.fixedRotation;
		particleBodyDef.type = b2BodyType::b2_dynamicBody;

		auto pBody = world->CreateBody(&particleBodyDef);
		pBody->CreateFixture(&particleFixtureDef);
		if (configure.type == STEffectType::Boom) {
			pBody->ApplyLinearImpulse(b2Vec2(
				std::cos(angle) * power / SCALE_RATIO,
				std::sin(angle) * power / SCALE_RATIO), pBody->GetWorldCenter() + 
				b2Vec2(size / SCALE_RATIO * d_powerPoint(gen), size / SCALE_RATIO * d_powerPoint(gen)), true);
		}
		
		// after duration, It will delete itself.
		auto wait = DelayTime::create(duration);
		auto opacity = FadeOut::create(0.5f);
		auto del = CallFunc::create([pBody, world]()->void {
			Node* p = (Node*)pBody->GetUserData();
			p->removeFromParent();
			world->DestroyBody(pBody);
		});
		auto sequence = Sequence::create(wait, opacity, del, nullptr);
		IngameScene::getInstance()->runLocalAction(sp, sequence);
		//sp->runAction(sequence);
		

		//bodies.emplace_back(pBody);

		
		
		
	}




}

void STEffectGenerator::generateEffect(const STEffectConfigure & configure, const float & x, const float & y)
{
	generateEffect(configure, Vec2(x, y));
}




