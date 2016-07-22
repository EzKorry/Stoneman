#include "DebugBox.h"


namespace arphomod {

DebugBox::Push DebugBox::push = Push();
bool DebugBox::init() {
	if (!Node::init()) return false;

	_height = 20.f;
	_max = 25;
	_fontSize = 15;

	return true;
}

DebugBox & DebugBox::operator<<(int arg)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	_stream << arg;
	return *this;
}

DebugBox & DebugBox::operator<<(const std::string & arg)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	_stream << arg;
	return *this;
}

DebugBox & DebugBox::operator<<(double arg)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	_stream << arg;
	return *this;
}

DebugBox & DebugBox::operator<<(DebugBox::Push arg)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	print();
	_stream.str("");
	return *this;
}

void DebugBox::print()
{
	TTFConfig conf("fonts/SpoqaHsRegular.ttf", _fontSize);
	auto label = Label::createWithTTF(conf, _stream.str());
	label->setAlignment(TextHAlignment::RIGHT);
	label->setAnchorPoint(Vec2(1.f,0.f));
	label->setPosition(0, 0);
	label->setTextColor(Color4B(0, 0, 0, 255));
	this->addChild(label);

	for (auto& p : _list) {
		p->setPositionY(p->getPositionY() + _height);
	}

	//제거
	if (_list.size() > _max) {
		_list.front()->removeFromParent();
		_list.pop_front();
	}
	//넣기
	_list.emplace_back(label);

}

DebugBox & DebugBox::get()
{
	return *this;
}

DebugBox::DebugBox()
{
}


DebugBox::~DebugBox()
{
}



}