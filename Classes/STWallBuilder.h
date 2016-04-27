#pragma once
#include "C:\cocos2d-x-project\Stoneman\cocos2d\cocos\2d\CCNode.h"
#include <cocos2d.h>
#include <unordered_map>

using namespace cocos2d;


// used to make walls.
// it should be added as child to master node.

struct stdnElement {
	int level = 0;
	cocos2d::Sprite* sprite = nullptr;
};
class STWallBuilder :public Node
{
public:
	STWallBuilder();
	CREATE_FUNC(STWallBuilder);
	void makeWall(const cocos2d::Rect& rect);

	// based coordinate. 1 = 20px, 
	// 0 = 0px ~ 20px, 1 = 20px ~ 40px
	void makeWall(int x, int y, int width, int height);


	const int maxBlockDarknessLevel = 6;
	virtual ~STWallBuilder();

private:
	//std::unordered_map<int, std::unordered_map<int, stdnElement>> _map;
	std::map<std::tuple<int, int>, stdnElement> _map;
	cocos2d::SpriteBatchNode* _batchNode{ nullptr };
	cocos2d::SpriteBatchNode* _black{ nullptr };
//	cocos2d::Node* _batchNode{ nullptr };
//	cocos2d::Node* _black{ nullptr };
	
};

