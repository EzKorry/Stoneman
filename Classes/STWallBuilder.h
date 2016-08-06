#pragma once
#include "C:\cocos2d-x-project\Stoneman\cocos2d\cocos\2d\CCNode.h"
#include <cocos2d.h>
#include <unordered_map>
#include <json/document.h>
#include <string>
#include <Box2D/Box2D.h>
using namespace rapidjson;
using namespace cocos2d;

// used to make walls.
// it should be added as child to master node.
enum SurfaceType {
	ST_void = 0x0000,
	ST_top_left = 0x0001,
	ST_top_right = 0x0002,
	ST_bottom_left = 0x0004,
	ST_bottom_right = 0x0008,
};
struct stdnElement {
	int level = 0;
	unsigned int type = ST_void;
	cocos2d::Sprite* sprite = nullptr;
};
class STWallBuilder :public Node
{
public:
	STWallBuilder();
	CREATE_FUNC(STWallBuilder);
	void makeWall(const cocos2d::Rect& rect);


	/*
		level 안에 레벨 이름, 그다음 블록들의 향연이 이어집니다.
		레벨 이름 안의 wall 속성 안의 것들은 배열로써,
		종류는 블록 그리기(꼭짓점기반), 점 옮기기(절대값) 등이 있습니다.
		type속성 :
		1. move_checkpoint : checkpoint로 이동.
		2. move_absolute : x, y 좌표값을 절대좌표로 이동.이동후 값을 checkpoint 속성으로 저장시킬 수 있음.
		3. move_relative : x, y 좌표값을 상대좌표로 이동.이동후 값을 checkpoint 속성으로 저장시킬 수 있음.
		4. solid_wall : 벽을 세움.상대좌표로 이동됨.tile속성을 추가로 줄 수 있음.이동후 값을 checkpoint 속성으로 저장시킬 수 있음.
		5. breakable_wall : 부술수 있는 벽. 돌진으로 부서짐.

	",*/
	void makeWalls(rapidjson::Document& j, const std::string & level);
	bool init() override;
	static const float surfaceGlowRatio;
	// based coordinate. 1 = 20px, 
	// 0 = 0px ~ 20px, 1 = 20px ~ 40px
	void makeWall(int x, int y, int width, int height);


	const int maxBlockDarknessLevel = 6;
	virtual ~STWallBuilder();

private:
	//std::unordered_map<int, std::unordered_map<int, stdnElement>> _map;
	std::map<std::tuple<int, int>, stdnElement> _map;
	std::map<b2Fixture*, std::vector<Sprite*>> _getSpritesByFixture;
	cocos2d::SpriteBatchNode* _batchNode{ nullptr };
	std::map<std::string, cocos2d::SpriteBatchNode*> _batchNodeMap;
	cocos2d::SpriteBatchNode* _black{ nullptr };
	cocos2d::RenderTexture* _target{ nullptr };
//	cocos2d::Node* _batchNode{ nullptr };
//	cocos2d::Node* _black{ nullptr };
	
};

