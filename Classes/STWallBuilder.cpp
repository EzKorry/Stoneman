#include "STWallBuilder.h"
#include "IngameScene.h"
#include <boost/coroutine/all.hpp>
#include "apAsyncTaskManager.h"


STWallBuilder::STWallBuilder()
{
}


void STWallBuilder::makeWall(const cocos2d::Rect & rect)
{
	
}

void STWallBuilder::makeWall(int x, int y, int width, int height)
{

	
	auto& doTask = apAsyncTaskManager::getInstance()->getTask();
	boost::coroutines::symmetric_coroutine<void>::call_type gogo(
		[this, x, y, width, height, &doTask](boost::coroutines::symmetric_coroutine<void>::yield_type& yield) {
		int n = 0;
		cocos2d::log("moved gogo1");
		if (_batchNode == nullptr) {
			//_batchNode = Node::create();
			_batchNode = SpriteBatchNode::create("tile1.png");
			_batchNode->setPosition(Vec2::ZERO);
			this->addChild(_batchNode, 10);
		}

		if (_black == nullptr) {
			//_black = Node::create();
			_black = SpriteBatchNode::create("black.png");
			_black->setPosition(Vec2::ZERO);
			this->addChild(_black, 5);
		}

		auto world = IngameScene::getb2World();



		auto pxRatio = IngameScene::OneBlockPx;
		auto px = x * pxRatio;
		auto py = y * pxRatio;
		auto w = width * pxRatio;
		auto h = height * pxRatio;



		// setting sprite.
		auto wallMasterNode = Node::create();
		wallMasterNode->setAnchorPoint(Vec2(0.5f, 0.5f));
		this->addChild(wallMasterNode);



		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				auto sp = Sprite::create("tile1.png");
				sp->setScale(pxRatio / sp->getContentSize().width);
				sp->setPosition(i * pxRatio + pxRatio / 2 + px, j * pxRatio + pxRatio / 2 + py);
				sp->setCascadeOpacityEnabled(true);
				//map[x + i][y + j].sprite = sp;
				_map[make_tuple(x + i, y + j)].sprite = sp;
				_batchNode->addChild(sp);
				//sp->setVisible(false);

				auto black = Sprite::create("black.png");
				black->setScale(pxRatio / black->getContentSize().width);
				black->setPosition(i * pxRatio + pxRatio / 2 + px, j * pxRatio + pxRatio / 2 + py);
				_black->addChild(black);
				//black->setVisible(false);


			}
			cocos2d::log("yield before%d", ++n);
			//yield(doTask);
			yield();
			cocos2d::log("yield after%d",n);
		}

		/// RESOLVING MAP!! --- set each block's opacity by depth.
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				//map[x + i][y + j].level = 1;
				_map[make_tuple(x + i, y + j)].level = 1;

			}
			
		}
		cocos2d::log("yield before%d", ++n);
		//yield(doTask);
		yield();
		cocos2d::log("yield after%d", n);

		for (int i = 1; i <= maxBlockDarknessLevel; i++) { // max level is 4.
			for (auto j = _map.begin(); j != _map.end(); j++) {
				//auto row = j->second;
				//for (auto k = row.begin(); k != row.end(); k++) {

				//auto cx = j->first;
				//auto cy = k->first;

				auto cx = std::get<0>(j->first);
				auto cy = std::get<1>(j->first);

				bool shouldBeDo = true;
				if (_map[make_tuple(cx, cy)].level > 0) { // apply for existing wall. level 0 == no wall.
					for (auto xx = cx - 1; xx <= cx + 1; xx++) {
						for (auto yy = cy - 1; yy <= cy + 1; yy++) { // for 9 blocks surrounding current one.
							if (_map[make_tuple(xx, yy)].level < i - 1) {
								shouldBeDo = false;
							}
						}
					}

					if (shouldBeDo) {
						auto coor = make_tuple(cx, cy);
						_map[coor].level = i;
						auto f = (i - 1) / ((float)maxBlockDarknessLevel - 1);
						//f = std::pow(f, 0.4f);
						_map[coor].sprite->setOpacity(255 - f * 255);
					}
				}
				cocos2d::log("yield before%d", ++n);
				//yield(doTask);
				yield();
				cocos2d::log("yield after%d", n);
				//}
			}
		}



		// Edge shape with virtual vertex.
		b2PolygonShape floorShape;
		floorShape.SetAsBox(w / SCALE_RATIO / 2, h / SCALE_RATIO / 2);

		b2FixtureDef floorFixture;
		floorFixture.density = 1.0f;
		floorFixture.friction = 0.5f;
		floorFixture.restitution = 0;
		floorFixture.shape = &floorShape;

		b2BodyDef floorBodyDef;
		floorBodyDef.position.Set(px / SCALE_RATIO, py / SCALE_RATIO);
		floorBodyDef.userData = wallMasterNode;
		floorBodyDef.type = b2BodyType::b2_staticBody;
		auto wallBody = world->CreateBody(&floorBodyDef);

		b2EdgeShape edgeShape;
		b2FixtureDef myFixtureDef;
		myFixtureDef.shape = &edgeShape;

		b2Vec2 leftBottom(0 / SCALE_RATIO, 0 / SCALE_RATIO);
		b2Vec2 leftTop(0 / SCALE_RATIO, h / SCALE_RATIO);
		b2Vec2 rightBottom(w / SCALE_RATIO, 0 / SCALE_RATIO);
		b2Vec2 rightTop(w / SCALE_RATIO, h / SCALE_RATIO);

		// left
		edgeShape.Set(leftBottom, leftTop);
		edgeShape.m_vertex0.Set(leftBottom.x, leftBottom.y - 1);
		edgeShape.m_vertex3.Set(leftTop.x, leftTop.y + 1);
		edgeShape.m_hasVertex0 = true;
		edgeShape.m_hasVertex3 = true;
		wallBody->CreateFixture(&myFixtureDef);

		// top
		edgeShape.Set(leftTop, rightTop);
		edgeShape.m_vertex0.Set(leftTop.x - 1, leftTop.y);
		edgeShape.m_vertex3.Set(rightTop.x + 1, leftTop.y);
		edgeShape.m_hasVertex0 = true;
		edgeShape.m_hasVertex3 = true;
		wallBody->CreateFixture(&myFixtureDef);

		// right
		edgeShape.Set(rightTop, rightBottom);
		edgeShape.m_vertex0.Set(rightTop.x, rightTop.y + 1);
		edgeShape.m_vertex3.Set(rightBottom.x, rightBottom.y - 1);
		edgeShape.m_hasVertex0 = true;
		edgeShape.m_hasVertex3 = true;
		wallBody->CreateFixture(&myFixtureDef);

		// bottom
		edgeShape.Set(leftBottom, rightBottom);
		edgeShape.m_vertex0.Set(leftBottom.x - 1, leftBottom.y);
		edgeShape.m_vertex3.Set(rightBottom.x + 1, rightBottom.y);
		edgeShape.m_hasVertex0 = true;
		edgeShape.m_hasVertex3 = true;
		wallBody->CreateFixture(&myFixtureDef);
		//_walls.emplace_back(wallBody);

	});

	apAsyncTaskManager::getInstance()->addTask(std::move(gogo));
	


	/*
	if (_batchNode == nullptr) {
		//_batchNode = Node::create();
		_batchNode = SpriteBatchNode::create("tile1.png");
		_batchNode->setPosition(Vec2::ZERO);
		this->addChild(_batchNode, 10);
	}

	if (_black == nullptr) {
		//_black = Node::create();
		_black = SpriteBatchNode::create("black.png");
		_black->setPosition(Vec2::ZERO);
		this->addChild(_black, 5);
	}

	auto world = IngameScene::getb2World();

	

	auto pxRatio = IngameScene::OneBlockPx;
	auto px = x * pxRatio;
	auto py = y * pxRatio;
	auto w = width * pxRatio;
	auto h = height * pxRatio;



	// setting sprite.
	auto wallMasterNode = Node::create();
	wallMasterNode->setAnchorPoint(Vec2(0.5f,0.5f));
	this->addChild(wallMasterNode);
	

	
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			auto sp = Sprite::create("tile1.png");
			sp->setScale(pxRatio / sp->getContentSize().width);
			sp->setPosition(i * pxRatio + pxRatio / 2 + px, j * pxRatio + pxRatio / 2 + py);
			sp->setCascadeOpacityEnabled(true);
			//map[x + i][y + j].sprite = sp;
			_map[make_tuple(x + i, y + j)].sprite = sp;
			_batchNode->addChild(sp);
			//sp->setVisible(false);

			auto black = Sprite::create("black.png");
			black->setScale(pxRatio / black->getContentSize().width);
			black->setPosition(i * pxRatio + pxRatio / 2 + px, j * pxRatio + pxRatio / 2 + py);
			_black->addChild(black);
			//black->setVisible(false);


		}
	}

	/// RESOLVING MAP!! --- set each block's opacity by depth.
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			//map[x + i][y + j].level = 1;
			_map[make_tuple(x + i, y + j)].level = 1;
			
		}
	}


	for (int i = 1; i <= maxBlockDarknessLevel; i++) { // max level is 4.
		for (auto j = _map.begin(); j != _map.end(); j++) {
			//auto row = j->second;
			//for (auto k = row.begin(); k != row.end(); k++) {

			//auto cx = j->first;
			//auto cy = k->first;

			auto cx = std::get<0>(j->first);
			auto cy = std::get<1>(j->first);

			bool shouldBeDo = true;
			if (_map[make_tuple(cx, cy)].level > 0) { // apply for existing wall. level 0 == no wall.
				for (auto xx = cx - 1; xx <= cx + 1; xx++) {
					for (auto yy = cy - 1; yy <= cy + 1; yy++) { // for 9 blocks surrounding current one.
						if (_map[make_tuple(xx, yy)].level < i - 1) {
							shouldBeDo = false;
						}
					}
				}

				if (shouldBeDo) {
					auto coor = make_tuple(cx, cy);
					_map[coor].level = i;
					auto f = (i - 1) / ((float)maxBlockDarknessLevel - 1);
					//f = std::pow(f, 0.4f);
					_map[coor].sprite->setOpacity(255 - f * 255);
				}
			}
			//}
		}
	}

	

	// Edge shape with virtual vertex.
	b2PolygonShape floorShape;
	floorShape.SetAsBox(w / SCALE_RATIO / 2, h / SCALE_RATIO / 2);

	b2FixtureDef floorFixture;
	floorFixture.density = 1.0f;
	floorFixture.friction = 0.5f;
	floorFixture.restitution = 0;
	floorFixture.shape = &floorShape;

	b2BodyDef floorBodyDef;
	floorBodyDef.position.Set(px / SCALE_RATIO, py / SCALE_RATIO);
	floorBodyDef.userData = wallMasterNode;
	floorBodyDef.type = b2BodyType::b2_staticBody;
	auto wallBody = world->CreateBody(&floorBodyDef);

	b2EdgeShape edgeShape;
	b2FixtureDef myFixtureDef;
	myFixtureDef.shape = &edgeShape;

	b2Vec2 leftBottom(0 / SCALE_RATIO, 0 / SCALE_RATIO);
	b2Vec2 leftTop(0 / SCALE_RATIO, h / SCALE_RATIO);
	b2Vec2 rightBottom(w / SCALE_RATIO, 0 / SCALE_RATIO);
	b2Vec2 rightTop(w / SCALE_RATIO, h / SCALE_RATIO);

	// left
	edgeShape.Set(leftBottom, leftTop);
	edgeShape.m_vertex0.Set(leftBottom.x, leftBottom.y - 1);
	edgeShape.m_vertex3.Set(leftTop.x, leftTop.y + 1);
	edgeShape.m_hasVertex0 = true;
	edgeShape.m_hasVertex3 = true;
	wallBody->CreateFixture(&myFixtureDef);

	// top
	edgeShape.Set(leftTop, rightTop);
	edgeShape.m_vertex0.Set(leftTop.x - 1, leftTop.y);
	edgeShape.m_vertex3.Set(rightTop.x + 1, leftTop.y);
	edgeShape.m_hasVertex0 = true;
	edgeShape.m_hasVertex3 = true;
	wallBody->CreateFixture(&myFixtureDef);

	// right
	edgeShape.Set(rightTop, rightBottom);
	edgeShape.m_vertex0.Set(rightTop.x, rightTop.y + 1);
	edgeShape.m_vertex3.Set(rightBottom.x, rightBottom.y - 1);
	edgeShape.m_hasVertex0 = true;
	edgeShape.m_hasVertex3 = true;
	wallBody->CreateFixture(&myFixtureDef);

	// bottom
	edgeShape.Set(leftBottom, rightBottom);
	edgeShape.m_vertex0.Set(leftBottom.x-1, leftBottom.y);
	edgeShape.m_vertex3.Set(rightBottom.x + 1, rightBottom.y);
	edgeShape.m_hasVertex0 = true;
	edgeShape.m_hasVertex3 = true;
	wallBody->CreateFixture(&myFixtureDef);
	//_walls.emplace_back(wallBody);
	*/
}

STWallBuilder::~STWallBuilder()
{
}


