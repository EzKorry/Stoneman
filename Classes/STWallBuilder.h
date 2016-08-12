#pragma once
#include "C:\cocos2d-x-project\Stoneman\cocos2d\cocos\2d\CCNode.h"
#include <cocos2d.h>
#include <unordered_map>
#include <json/document.h>
#include <string>
#include <Box2D/Box2D.h>
#include <array>
#include <tuple>
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
enum class WallType {
	Solid,
	Breakable
};

enum UpdateWallSprite {
	UW_X,
	UW_Y
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
	void setCorrectTex(cocos2d::Sprite* sp);
	bool init() override;
	static const float surfaceGlowRatio;
	// based coordinate. 1 = 20px, 
	// 0 = 0px ~ 20px, 1 = 20px ~ 40px
	void makeWall(int x, int y, int width, int height);
	void changeWallStatus(b2Body* body, WallType status);
	void breakWall(b2Body* body);
	void breakWall(b2Fixture* fixture);
	void waitForUpdateVecRect();

	// It used when making clipping node.
	cocos2d::SpriteBatchNode* getBatchSpriteNode(const std::string& path);

	const int maxBlockDarknessLevel = 6;
	virtual ~STWallBuilder();

private:
	//std::unordered_map<int, std::unordered_map<int, stdnElement>> _map;
	std::map<std::tuple<int, int>, stdnElement> _map;
	std::map<b2Body*, std::vector<Sprite*>> _getSpritesByBody;
	std::map<b2Body*, std::vector<Sprite*>> _getCrackByBody;
	std::map<b2Body*, WallType> _wallStatus;
	
	//std::map<b2Fixture*, b2Body*> _fixtureToBody;
	cocos2d::SpriteBatchNode* _batchNode{ nullptr };
	std::map<std::string, cocos2d::SpriteBatchNode*> _batchNodeMap;
	cocos2d::SpriteBatchNode* _batch{ nullptr };
	cocos2d::SpriteBatchNode* _black{ nullptr };
	cocos2d::RenderTexture* _target{ nullptr };

	// using for sprite update
	int _mapArrayWidth{ 0 };
	int _mapArrayHeight{ 0 };
	int _nowMasterFieldX = 0;
	int _nowMasterFieldY = 0;

	int _waitForUpdateVecRect = 20;
	int _waitForUpdateVecRectMax = 20;

	//(0,0) ~ (47, 26) : normal
	//array[48][27] : edge
	std::array<std::array<Sprite*, 49>, 49> _mapSprites;
	std::vector<std::tuple<Rect, b2Body*>> _vecRect;
	std::map<int, std::map<int, Rect&>> _coordRect;
	//std::vector<b2Body*> _sortedByDistance;

	const std::string crackedSpritePath = "img/cracked.png";
//	cocos2d::Node* _batchNode{ nullptr };
//	cocos2d::Node* _black{ nullptr };
	
};

