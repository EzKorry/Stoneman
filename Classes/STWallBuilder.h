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
		level �ȿ� ���� �̸�, �״��� ��ϵ��� �⿬�� �̾����ϴ�.
		���� �̸� ���� wall �Ӽ� ���� �͵��� �迭�ν�,
		������ ��� �׸���(���������), �� �ű��(���밪) ���� �ֽ��ϴ�.
		type�Ӽ� :
		1. move_checkpoint : checkpoint�� �̵�.
		2. move_absolute : x, y ��ǥ���� ������ǥ�� �̵�.�̵��� ���� checkpoint �Ӽ����� �����ų �� ����.
		3. move_relative : x, y ��ǥ���� �����ǥ�� �̵�.�̵��� ���� checkpoint �Ӽ����� �����ų �� ����.
		4. solid_wall : ���� ����.�����ǥ�� �̵���.tile�Ӽ��� �߰��� �� �� ����.�̵��� ���� checkpoint �Ӽ����� �����ų �� ����.
		5. breakable_wall : �μ��� �ִ� ��. �������� �μ���.

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

