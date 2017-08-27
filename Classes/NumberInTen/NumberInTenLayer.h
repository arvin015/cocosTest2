//
// Created by Administrator on 2017/8/26.
//

#ifndef PROJ_ANDROID_STUDIO_NUMBERINTENLAYER_H
#define PROJ_ANDROID_STUDIO_NUMBERINTENLAYER_H

#include <stdio.h>
#include "../Main/BaseLayer.h"
#include "ui/UIImageView.h"

class DragImageView;

class NumberInTenLayer : public BaseLayer {

public:

    enum HotRectType {
        BIG,
        SMALL
    };

    NumberInTenLayer();
    ~NumberInTenLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(NumberInTenLayer);

    /**
     * 设置是否可拖拽
     */
    bool setDragEnabled(bool isDragEnabled) {
        this->isDragEnabled = isDragEnabled;
    };

    /**
     * 设置热区
     */
    void setHotRect(HotRectType type);

private:

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    /**
     * 按钮点击事件
     */
    void onBtnClick(cocos2d::Ref* pSender);

    /**
     * 设置操作图标
     * @param page
     */
    void setDragIcons(int page);

    /**
     * 创建Temp拖拽图标
     */
    cocos2d::Sprite* createTempSprite(const std::string &picStr, const cocos2d::Vec2 &position);

    /**
     * 创建拖拽图标
     */
    DragImageView* createDragImageView(const std::string &picStr, const cocos2d::Vec2 &position);

    /**
     * 检测拖拽图标是否落在热区中
     */
    bool checkInHotRect(cocos2d::Sprite* dragSprite);

    /**
     * 越界处理
     */
    void checkIsOutBorder(cocos2d::Node* dragNode);

private:

    enum BTN_TAGS {
        PREV_BTN = 111,
        NEXT_BTN
    };

    cocos2d::EventListenerTouchOneByOne* eventListener;
    bool isDragEnabled;
    cocos2d::Rect hotRect; //活动区域
    cocos2d::ui::Button* prevBtn;
    cocos2d::ui::Button* nextBtn;
    cocos2d::Vector<cocos2d::ui::ImageView*> dragImageList; //操作栏拖拽图标集
    std::string dragImageStr; //当前拖拽的图标名
    cocos2d::Sprite* tempSprite; //当前copy的拖拽图标
    std::map<std::string, cocos2d::Vector<DragImageView*>> dragImageViewMap; //创建的拖拽图标集
    int currentPage; //当前页
};


#endif //PROJ_ANDROID_STUDIO_NUMBERINTENLAYER_H
