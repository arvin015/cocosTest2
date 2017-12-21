#include "AndroidModelUtils.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MainLayer.h"
#include "NavigationController.h"

USING_NS_CC;
using namespace ui;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    LayerColor* bg = LayerColor::create(Color4B::WHITE);
    addChild(bg);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
//    auto label = Label::createWithTTF("Hello World111111", "fonts/Marker Felt.ttf", 24);
//    
//    // position the label on the center of the screen
//    label->setPosition(Vec2(origin.x + visibleSize.width/2,
//                            origin.y + visibleSize.height - label->getContentSize().height));
//
//    // add the label as a child to this layer
//    this->addChild(label, 1);
//
//    // add "HelloWorld" splash screen"
//    auto sprite = Sprite::create("HelloWorld.png");
//
//    // position the sprite on the center of the screen
//    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
//
//    // add the sprite as a child to this layer
//    this->addChild(sprite, 0);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    log("--------------- brand = %s, device = %s, model = %s", AndroidModelUtils::getAndroidModel()[0].c_str(),
        AndroidModelUtils::getAndroidModel()[1].c_str(), AndroidModelUtils::getAndroidModel()[2].c_str());
#endif
    
    rootLayer = RootLayer::create();
    this->addChild(rootLayer);

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    auto eventKeyboard = EventListenerKeyboard::create();
    eventKeyboard->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event){
        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
            Director::getInstance()->end();
        }
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventKeyboard, this);
#endif

    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}

RootLayer::RootLayer() {

}

RootLayer::~RootLayer() {

}
    
bool RootLayer::init() {
    if(!Layer::init()) {
        return false;
    }
    
    controller = NavigationController::create();
    addChild(controller);
    
    MainLayer* mainLayer = MainLayer::create();
    controller->pushView(mainLayer);
    
    return true;
}
