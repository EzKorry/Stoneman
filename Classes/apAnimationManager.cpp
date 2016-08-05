#include "apAnimationManager.h"
#include <cmath>
#include "IngameScene.h"

namespace arphomod {

void apAnimationManager::connectWithScheduler(cocos2d::Scheduler * scheduler)
{
	if (_scheduleTarget == nullptr) {
		_scheduleTarget = new (std::nothrow) ScheduleTarget();
	}
	if (_scheduler != nullptr) {
		_scheduler->unschedule("apAnimationManager", _scheduleTarget);
	}

	_scheduler = scheduler;

	_scheduler->schedule([this](float delta)->void {
		step();
	}, _scheduleTarget, 1 / 15.f, false, "apAnimationManager");
}
void apAnimationManager::addNewSpriteSheet(const std::string & plistPath)
{
	auto spriteCache = SpriteFrameCache::getInstance();
	spriteCache->addSpriteFramesWithFile(plistPath);
}
void apAnimationManager::setFrameEvent(const std::string & animationName, int index, const std::string & hookName)
{
	auto animationCache = AnimationCache::getInstance();
	auto animation = animationCache->getAnimation(animationName);

	// exit if animation is null.
	if (!animation) return;

	auto num = animation->getTotalDelayUnits();

	// exit if index is bigger than animation frame counts.
	if (std::abs(index) > num) return;

	if (index == 0) return;
	else if (index < 0) {
		index = num - index;
	}
	else {
		index = index - 1;
	}

	auto animationFrames = animation->getFrames();
	auto& valueMap = animationFrames.at(index)->getUserInfo();
	valueMap["hook_name"] = hookName;
}
void apAnimationManager::specifyAnimation(const char * format, const FrameRange & frameRange, const std::string & name)
{
	auto spritecache = SpriteFrameCache::getInstance();
	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = frameRange.start; i <= frameRange.end; i++)
	{
		sprintf(str, format, i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	auto animation = Animation::createWithSpriteFrames(animFrames, 1 / 15.f, 1);
	auto animationCache = AnimationCache::getInstance();
	animationCache->addAnimation(animation, name);
}
void apAnimationManager::playAnimation(Sprite * sprite, const std::string & animationName, bool isLoop)
{
	if (_playingAnimations.find(sprite) != _playingAnimations.end()) {
		_playingAnimations[sprite].animationName = animationName;
		_playingAnimations[sprite].nowFrame = 0U;
	}
	else if (sprite) {
		CC_SAFE_RETAIN(sprite);
		_playingAnimations.emplace(sprite, PlayingAnimation(animationName, 0U));
	}
	



}

void apAnimationManager::stopAnimation(Sprite * sprite)
{
	if (sprite && _playingAnimations.find(sprite) != _playingAnimations.end()) {
		_playingAnimations[sprite].animationName = "";
		_playingAnimations[sprite].nowFrame = 0U;
	}
}

Sprite* apAnimationManager::createSprite(const std::string & animationName)
{
	auto animation = AnimationCache::getInstance()->getAnimation(animationName);
	if (animation) {
		return Sprite::createWithSpriteFrame(animation->getFrames().front()->getSpriteFrame());
	}
	return nullptr;
	
}
void apAnimationManager::step()
{
	auto animationCache = AnimationCache::getInstance();
	auto hookActionManager = apHookActionManager::getInstance();

	std::vector<Sprite*> tobeRemoved;
	for (auto& item : _playingAnimations) {
		auto& sprite = item.first;
		auto& aniInfo = item.second;
		auto& name = aniInfo.animationName;

		if (animationCache->getAnimation(name)) {


			auto& frames = animationCache->getAnimation(name)->getFrames();
			auto& nowFrame = aniInfo.nowFrame;
			size_t size = frames.size();

			// end animation
			if (nowFrame >= size) {
				tobeRemoved.emplace_back(sprite);
			}

			// go animation
			else {
				sprite->setDisplayFrameWithAnimationName(name, nowFrame);

				// hook action execute.
				auto& userInfo = frames.at(nowFrame)->getUserInfo();
				if (userInfo.find("hook_name") != userInfo.end()) {
					hookActionManager->runHook(userInfo.at("hook_name").asString());
				}


				nowFrame++;
			}
		}
	}
	
	for (auto& item : tobeRemoved) {
		stopAnimation(item);
	}

}

apAnimationManager::apAnimationManager()
{

}


apAnimationManager::~apAnimationManager()
{
	CC_SAFE_RELEASE(_scheduleTarget);

	for (auto& item : _playingAnimations) {
		CC_SAFE_RELEASE(item.first);
	}
}

FrameRange::FrameRange(int start, int end) :
	start(start), end(end) {}
FrameRange::FrameRange() : start(0), end(0)
{

}
FrameRange::~FrameRange()
{
}

shared_ptr<apAnimationManager> apAnimationManager::_sp = nullptr;

} // namespace arphomod