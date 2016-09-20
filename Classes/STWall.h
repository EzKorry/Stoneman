#ifndef _STWALL_H_
#define _STWALL_H_

#include <cocos2d.h>
#include <Box2D\Box2D.h>
#include <string>

enum class WallType {
	Solid,
	Breakable,
	ButtonOn,
	ButtonOff,
	Teleport
};

enum class WallButtonActionType {
	TurnToBreakableWall,
	TurnToSolidWall,
	TurnButtonOn,
	TurnButtonOff
};

enum UpdateWallSprite {
	UW_X,
	UW_Y
};

using namespace cocos2d;
using namespace std;

class STWall : public cocos2d::Ref
{
public:
	static STWall * create(
		const std::string& name, 
		WallType wallType, 
		const Rect& rect, 
		b2Body* body);

	bool init();

	// getter.
	inline std::string getName();
	inline WallType getWallType();
	inline Rect getRect();
	/*inline std::vector<b2Fixture*> getFixtures();*/
	inline b2Body* getBody();

	void runAction(WallButtonActionType type, void* object);


	// setter
	void setWallType(WallType wallType);
	
	virtual ~STWall();
protected:
	STWall(const std::string& name,
		WallType wallType,
		const Rect& rect,
		b2Body* body);
private:
	const std::string _name;
	WallType _wallType;
	const Rect _rect;
	b2Body* const _body;
	std::map<WallButtonActionType, std::string> _wall

};

#endif