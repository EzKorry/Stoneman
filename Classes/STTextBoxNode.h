#ifndef _STTEXTBOXNODE_H_
#define _STTEXTBOXNODE_H_

#include <cocos2d.h>
#include <string>
class b2Body;
using namespace std;
using namespace cocos2d;

class STTextBoxNode : public Node {
public:
	CREATE_FUNC(STTextBoxNode);

	Node* createTextBox(const string& text, const Vec2& vec);
	void createFollowingTextBox(const string& text, b2Body* body, const Vec2& offset);
protected:
	STTextBoxNode();
	~STTextBoxNode();
};


#endif
