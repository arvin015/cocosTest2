//
// Created by arvin on 2017/8/16.
//

#ifndef PROJ_ANDROID_STUDIO_UIDRAWVIEW_H
#define PROJ_ANDROID_STUDIO_UIDRAWVIEW_H

#include "cocos2d.h"

class UIDrawView : public cocos2d::Layer {

public:

    enum DrawType {
        DRAW,
        CLEAR
    };
    
    UIDrawView();
    ~UIDrawView();
    
    virtual void onEnter();
    virtual void onExit();
    
    CREATE_FUNC(UIDrawView);
    virtual bool init();
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    void setDrawType(DrawType drawType);

private:

    void onDraw(cocos2d::Vec2 from, cocos2d::Vec2 to);

private:
    cocos2d::EventListenerTouchOneByOne* eventListener;
    cocos2d::Vec2 lastPos;
    cocos2d::RenderTexture* rt;
    DrawType drawType;
};


#endif //PROJ_ANDROID_STUDIO_UIDRAWVIEW_H
