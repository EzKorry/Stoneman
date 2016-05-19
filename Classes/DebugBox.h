#ifndef _DEBUG_BOX_H_
#define _DEBUG_BOX_H_
#include <cocos2d.h>
#include <sstream>
#include <list>
USING_NS_CC;
namespace arphomod {

/********************************
* example:
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto debugBox = DebugBox::create();
	addChild(debugBox, 999);
	debugBox->setAnchorPoint(Vec2::ZERO);
	debugBox->setPosition(Vec2(visibleSize.width, 0.f));
	debugBox->get() << "abce" << 123 << 1.234 << DebugBox::push;
	debugBox->get() << "abc2" << 223 << 1.234 << DebugBox::push;
	debugBox->get() << "abc4" << 323 << 1.234 << DebugBox::push;

* DebugBox::init() has configures.
**********************************************************/
class DebugBox : public cocos2d::Node
{
private:
	class Push;
public:
	
	
	CREATE_FUNC(DebugBox);
	static Push push;
	bool init() override;
	
	DebugBox& operator<<(int arg);
	DebugBox& operator<<(const std::string& arg);
	DebugBox& operator<<(double arg);
	DebugBox& operator<<(DebugBox::Push arg);
	void print();
	DebugBox& get();

	DebugBox();
	virtual ~DebugBox();

private:
	std::list<Node*> _list;
	std::stringstream _stream;
	float _height = 30.f;
	int _max = 25;
	int _fontSize = 15;

	class Push {
	public:
		explicit Push() {};
		~Push() {};
	private:
	};

};

}

#endif

