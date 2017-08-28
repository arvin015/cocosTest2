//
// Created by Administrator on 2017/8/26.
//

#ifndef PROJ_ANDROID_STUDIO_NUMBERINTENLAYER_H
#define PROJ_ANDROID_STUDIO_NUMBERINTENLAYER_H

#include <stdio.h>
#include "BaseLayer.h"
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
    void setDragEnabled(bool isDragEnabled) {
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
     * 创建操作图案
     * @param page
     */
    void initDragIcons();
    
    /**
     * 切换操作图案
     */
    void switchDragIcons(int page);

    /**
     * 创建Temp拖拽图案
     */
    cocos2d::Sprite* createTempSprite(const std::string &picStr, const cocos2d::Vec2 &position);

    /**
     * 创建拖拽图案
     */
    DragImageView* createDragImageView(const std::string &picStr, const cocos2d::Vec2 &position);

    /**
     * 检测拖拽图案是否落在热区中
     */
    bool checkInHotRect(cocos2d::Sprite* dragSprite);

    /**
     * 越界处理
     */
    void checkIsOutBorder(cocos2d::Node* dragNode);
    
    /**
     * 获取最大的zOrder
     */
    int getMaxZOrder();

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
    cocos2d::Vector<cocos2d::ui::ImageView*> dragImageList; //操作栏拖拽图案集
    cocos2d::ui::ImageView* seletedImage; //选中的图案
    cocos2d::Sprite* tempSprite; //当前copy的拖拽图案
    std::map<std::string, cocos2d::Vector<DragImageView*>> dragImageViewMap; //创建的拖拽图案集
    int currentPage; //当前页
};


#endif //PROJ_ANDROID_STUDIO_NUMBERINTENLAYER_H
