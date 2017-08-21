#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/UIRadioButton.h"
#include "DatasetInfo.h"

class RootLayer;
class DatasetTableView;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    RootLayer* getRootLayer() {
        return rootLayer;
    }
    
private:
    RootLayer* rootLayer;
};

class NavigationController;
class RootLayer : public cocos2d::Layer {
  
public:
    RootLayer();
    ~RootLayer();
    
    virtual bool init();
    
    CREATE_FUNC(RootLayer);
    
public:
    NavigationController* controller;
};

#endif // __HELLOWORLD_SCENE_H__
