#include "STWallBuilder.h"
#include "IngameScene.h"
#include <boost/coroutine/all.hpp>
#include "apAsyncTaskManager.h"

const float STWallBuilder::surfaceGlowRatio = 0.1f;
STWallBuilder::STWallBuilder()
{
}


void STWallBuilder::makeWall(const cocos2d::Rect & rect)
{
	
}

void STWallBuilder::makeWalls(const json & j, const std::string & level)
{
	_map.clear();
	if (_target != nullptr) {
		_target->removeFromParent();
		_target = nullptr;
	}
	auto ss = j["level"];
	auto s = j["level"]["1"]["size"];
	auto pxRatio = IngameScene::OneBlockPx;
	_target = RenderTexture::create(s["w"] * pxRatio, s["h"] * pxRatio, Texture2D::PixelFormat::RGBA8888);
	_target->setPosition(s["w"] * pxRatio / 2, s["h"] * pxRatio / 2);
	this->addChild(_target, -1);

	boost::coroutines::symmetric_coroutine<void>::call_type makeWallsCoroutine(
		[this, &j, level, pxRatio](boost::coroutines::symmetric_coroutine<void>::yield_type& yield) {
		
		auto makeBatchNode = [this](const std::string& path, int zIndex) {
			_batchNodeMap[path] = SpriteBatchNode::create(path);
			_batchNodeMap[path]->setPosition(Vec2::ZERO);
			this->addChild(_batchNodeMap[path], zIndex);
		};
		makeBatchNode("tile2.png", 5);
		makeBatchNode("tile3.png", 5);
		makeBatchNode("ground_bottom_left2.png", 10);
		auto n = 0;
		for (auto& map : j["level"][level]["walls"]) {
			auto world = IngameScene::getb2World();
			auto w_int = map["w"] + 0;
			auto h_int = map["h"] + 0;
			auto x_int = map["x"] + 0;
			auto y_int = map["y"] + 0;
			auto px = x_int * pxRatio;
			auto py = y_int * pxRatio;
			auto w = w_int * pxRatio;
			auto h = h_int * pxRatio;
			std::string tileName = map["tile"];
			tileName.append(".png");
			auto wallMasterNode = Node::create();
			wallMasterNode->setAnchorPoint(Vec2(0.5f, 0.5f));
			this->addChild(wallMasterNode);

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

			for (int i = x_int; i < x_int + w_int; i++) {
				//_target->begin();
				for (int j = y_int; j < y_int + h_int; j++) {
					
					auto sp = Sprite::create(tileName);
					//sp->retain();
					sp->setScale(pxRatio / sp->getContentSize().width);
					sp->setPosition((i + 1.f/2.f) * pxRatio , (j + 1.f/2.f) * pxRatio);
					_batchNodeMap[tileName]->addChild(sp);
					//sp->visit();
					auto index = make_tuple(i,j);
					_map[index].type = ST_void;
					_map[index].sprite = sp;
					//sp->setVisible(false);
					/*
					auto black = Sprite::create("black.png");
					black->setScale(pxRatio / black->getContentSize().width);
					black->setPosition(i * pxRatio + pxRatio / 2 + px, j * pxRatio + pxRatio / 2 + py);
					_black->addChild(black);*/
					//black->setVisible(false);
				}
				//_target->end();
				cocos2d::log("yield before%d", ++n);
				//yield(doTask);
				yield();
				cocos2d::log("yield after%d", n);
			}
		}
		auto makeSurface = [this, pxRatio](const stdnElement& e, const std::string& path, float rotate) {
			
			auto surf = Sprite::create(path);
			surf->setPosition(e.sprite->getPosition());
			surf->setCascadeOpacityEnabled(true);
			//surf->setOpacity(255.f * surfaceGlowRatio);
			surf->setRotation(rotate);
			surf->setLocalZOrder(10);
			surf->setScale(pxRatio / surf->getContentSize().width);
			//surf->visit();
			_batchNodeMap[path]->addChild(surf);
		};
		std::string surf_bottom_left_path = "ground_bottom_left2.png";

		/// RESOLVING MAP!! --- set each block's opacity by depth.
		for (auto& block : _map){
			auto& e = block.second;
			auto& now_x = std::get<0>(block.first);
			auto& now_y = std::get<1>(block.first);
			bool top_left = false;
			bool top_right = false;
			bool bottom_left = false;
			bool bottom_right = false;
			for (int ai = -1; ai < 2; ai++) {
				for (int aj = -1; aj < 2; aj++) {
					// if empty.
					if (_map.find(make_tuple(now_x + ai, now_y + aj)) == _map.end() &&
						now_x + ai <= j["level"][level]["size"]["w"] + 0 &&
						now_y + aj <= j["level"][level]["size"]["h"] + 0 &&
						now_x + ai >= 0 &&
						now_y + aj >= 0) {
						if (ai + aj < 0) bottom_left = true;
						if (ai + aj > 0) top_right = true;
						if (ai - aj > 0) bottom_right = true;
						if (ai - aj < 0) top_left = true;
					}
				}
			}
			//_target->begin();
			if (bottom_left) {
				e.type |= ST_bottom_left;
				makeSurface(e, surf_bottom_left_path, 0.f);
			}
			if (top_left) {
				e.type |= ST_top_left;
				makeSurface(e, surf_bottom_left_path, 90.f);
			}
			if (top_right) {
				e.type |= ST_top_right;
				makeSurface(e, surf_bottom_left_path, 180.f);
			}
			if (bottom_right) {
				e.type |= ST_bottom_right;
				makeSurface(e, surf_bottom_left_path, 270.f);
			}
			
			
			//e.sprite->release();
			//_target->end();
			cocos2d::log("yield before%d", ++n);
			//yield(doTask);
			yield();
			cocos2d::log("yield after%d", n);
		}
	});
	apAsyncTaskManager::getInstance()->addTask(std::move(makeWallsCoroutine));
}

bool STWallBuilder::init()
{
	if (!Node::init()) return false;

	


	return true;
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


