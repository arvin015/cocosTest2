//
// Created by Administrator on 2019/6/1.
//

#ifndef PROJ_ANDROID_STUDIO_POLYVIEW_H
#define PROJ_ANDROID_STUDIO_POLYVIEW_H

#include "cocos2d.h"
#include "UIDrawNodeEx.h"

class PolyView : public cocos2d::Node {
public:

    typedef std::function<void()> OnTouchEndCallback;

    enum PolyType {
        NONE,
        ARC,
        CIRCLE
    };

    PolyView();
    ~PolyView();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();
    CREATE_FUNC(PolyView);

    void createArc(const cocos2d::Vec2 &centerP, float radius);

    void createCircle(const cocos2d::Vec2 &centerP, float radius);

    bool checkIsCloseEnough(PolyView* otherPolyView);

    void attach(PolyView* otherPolyView);

    cocos2d::Vec2 getCenterP();

    PolyType getPolyType() {
        return polyType;
    };

    float getRadius() {
        return radius;
    }

    void setOnTouchEndListener(OnTouchEndCallback touchEndCallback) {
        this->touchEndCallback = touchEndCallback;
    }

private:

    /**
     * 检测触点是否在poly内
     */
    bool checkIsContainerPoint(const cocos2d::Vec2 &touchP);

    /**
     * 置顶
     */
    void bringToFront();

private:
    cocos2d::EventListenerTouchOneByOne* touchListener;
    DrawNodeEx* drawNodeEx;

    PolyType polyType;
    float radius;
    bool isSelect;

    OnTouchEndCallback touchEndCallback;
};


#endif //PROJ_ANDROID_STUDIO_POLYVIEW_H
