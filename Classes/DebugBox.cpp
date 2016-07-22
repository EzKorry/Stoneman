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
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	_stream << arg;
	return *this;
}

DebugBox & DebugBox::operator<<(const std::string & arg)
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	_stream << arg;
	return *this;
}

DebugBox & DebugBox::operator<<(double arg)
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	_stream << arg;
	return *this;
}

DebugBox & DebugBox::operator<<(DebugBox::Push arg)
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
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

	//����
	if (_list.size() > _max) {
		_list.front()->removeFromParent();
		_list.pop_front();
	}
	//�ֱ�
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