#ifndef _APANIMATIONMANAGER_H_
#define _APANIMATIONMANAGER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <cocos2d.h>

using namespace std;
using namespace cocos2d;

namespace arphomod {
	
struct FrameRange {
public:
	FrameRange(int start, int end);
	FrameRange();
	~FrameRange();
	int start = 0;
	int end = 0;
};
	



class apAnimationManager
{
public:

	struct PlayingAnimation {
		PlayingAnimation() {}
		PlayingAnimation(const std::string& name, size_t nowFrame) 
			: animationName(name), nowFrame(nowFrame) {

		}
		std::string animationName;
		size_t nowFrame;
	};

	class ScheduleTarget : public cocos2d::Ref {

	};


	static shared_ptr<apAnimationManager> getInstance() {
		if (_sp == nullptr) {
			_sp = make_shared<apAnimationManager>();
		}
		return _sp;
	}
	
	void connectWithScheduler(cocos2d::Scheduler* scheduler);

	void addNewSpriteSheet(const std::string& plistPath);

	// index#first : 1, 2, 3 ... -3, -2, -1 : last
	void setFrameEvent(const std::string& animationName, int index, const std::string& hookName);

	void specifyAnimation(const char * format, const FrameRange& frameRange, const std::string& name);

	void playAnimation(Sprite* sprite, const std::string& animationName, bool isLoop);

	void stopAnimation(Sprite* sprite);

	//return nullptr if animationName is invalid.
	Sprite* createSprite(const std::string& animationName);

	apAnimationManager();
	~apAnimationManager();

private:

	void step();
	
	static shared_ptr<apAnimationManager> _sp;
	std::string _nowPlaying;
	ScheduleTarget* _scheduleTarget{ nullptr };
	// unordered_map<SpritePointer, std::
	cocos2d::Scheduler* _scheduler{ nullptr };
	//std::unordered_map<std::string, cocos2d::Vector<SpriteFrame*>> _animations;

	//elements: sprite, tuple(animation name, nowFrame)
	std::unordered_map<cocos2d::Sprite*, PlayingAnimation> _playingAnimations;
	
};

} /* namespace arphomod */


#endif;