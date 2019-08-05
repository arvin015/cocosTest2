//
// Created by mac_arvin on 2019-08-02.
//

#ifndef PROJ_ANDROID_STUDIO_UIDRAGNODE_H
#define PROJ_ANDROID_STUDIO_UIDRAGNODE_H

#include "cocos2d.h"

class DragNode : public cocos2d::Node {
    typedef cocos2d::Node base;

public:

    typedef std::function<void(cocos2d::Vec2)> OnDragStart;
    typedef std::function<void(cocos2d::Vec2)> OnDragMove;
    typedef std::function<void(cocos2d::Vec2)> OnDragEnd;

    DragNode();
    virtual ~DragNode();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();
    CREATE_FUNC(DragNode);

    virtual void update(float dt);
    
    /**
     * 设置是否可拖拽
     */
    void setEnableDrag(bool enableDrag) {
        this->enableDrag = enableDrag;
    }

    /**
     * 获取是否可拖拽
     */
    bool isEnableDrag() {
        return enableDrag;
    }
    
    /**
     * 设置虚像位置
     * @param position
     */
    void setVirtualImagePosition(const cocos2d::Vec2 &position);

    void setOnDragStartListener(const OnDragStart &listener) {
        this->onDragStart = listener;
    }

    void setOnDragMoveListener(const OnDragMove &listener) {
        this->onDragMove = listener;
    }

    void setOnDragEndListener(const OnDragEnd &listener) {
        this->onDragEnd = listener;
    }

private:

    bool onTouchDown(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMove(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchUp(cocos2d::Touch* touch, cocos2d::Event* event);

    /**
     * 长按处理
     */
    void onLongClickHandle();

private:
    cocos2d::EventListenerTouchOneByOne* touchListener;

    OnDragStart onDragStart;
    OnDragMove onDragMove;
    OnDragEnd onDragEnd;

    bool enableDrag;
    
    bool longClickCounter;
    bool isLongClick;
    float timeCounter;

    cocos2d::RenderTexture* renderTexture; //虚像
    cocos2d::Vec2 beginWorldVec; //开始的世界坐标
};


#endif //PROJ_ANDROID_STUDIO_UIDRAGNODE_H
