#include "HelloWorldScene.h"
#include "STCamera.h"
#include "STTextField.h"
#include "apTouchManager.h"
#include "apHookActionManager.h"
#include "IngameScene.h"

USING_NS_CC;
using namespace arphomod;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 visibleOrigin = origin;


    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    auto masterNode = Node::create();
    this->addChild(masterNode);
    auto camera = STCamera::create();

/*
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));
    masterNode->addChild(label, 1);

    auto textfield = TextFieldTTF::textFieldWithPlaceHolder("000",Size(50.0f,50.0f),TextHAlignment::CENTER, "fonts/SpoqaHsThin.ttf",24.0f);
	textfield->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y - 100.0f));
	masterNode->addChild(textfield, 2, "textfield");
/*
	auto st_text = STTextField::create([](cocos2d::ui::EditBox* eb)->void{});
	st_text->setPosition(Vec2(visibleSize.width/2 , 150));
	masterNode->addChild(st_text, 3, "yooo");

    auto sprite = Sprite::create("HelloWorld.png");
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    masterNode->addChild(sprite, 0);

    this->addChild(masterNode);
    this->addChild(camera);
    
    camera->setField(masterNode);

    auto wait0 = DelayTime::create(1.5f);
    auto func1 = CallFunc::create([camera]()->void {
    	camera->setCameraPosition(Vec2(0.0f,0.0f));
    });
    auto wait1 = DelayTime::create(1.0f);
    auto func2 = CallFunc::create([camera]()->void {
    	camera->setCameraPosition(Vec2(480.0f/2.0f,270.0f/2.0f));
    });
    auto seq = Sequence::create(wait0, func1, wait1, func2, nullptr);
    auto infinite = RepeatForever::create(seq);
*/
    auto touchManager = APTouchManager::getInstance();
    auto nextlabel = Label::createWithTTF("go to the next stage!","fonts/SpoqaHsThin.ttf", 25);
    auto nextlabelsize = nextlabel->getContentSize();
    nextlabel->setPosition(visibleSize.width - 30, 30);
    nextlabel->setAlignment(TextHAlignment::CENTER);
    masterNode->addChild(nextlabel, 5, "nextlabel");


    auto checker = APTouchManager::createDefaultChecker(nextlabel);
    touchManager->registerNode(nextlabel, checker);
    touchManager->addBehavior(nextlabel, APTouchType::Began, []()->void {
    	auto director = Director::getInstance();
    	//auto ingameScene = IngameScene::create();
    	auto ingameScene = Scene::create();
    	//auto sprite = Sprite::create("bg.png");
    	//sprite->setScale(10.0f);
    	//ingameScene->addChild(ingameScene);
    	director->replaceScene(ingameScene);
    	cocos2d::log("clicked");
    },"goIngame","goIngameBeh");

    cocos2d::log("nextlebel reference count --> %d",nextlabel->getReferenceCount());



    //this->runAction(infinite);




    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
