//
// Created by Administrator on 2017/8/26.
//

#ifndef PROJ_ANDROID_STUDIO_NUMBERINTENLAYER_H
#define PROJ_ANDROID_STUDIO_NUMBERINTENLAYER_H

#include <stdio.h>
#include "BaseLayer.h"
#include "ui/UIImageView.h"

class PlayImageView;
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
    
    /**
     * 舞动
     */
    void dance();

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
    
    /**
     * 播放一组移位动画
     * @dragImageList 每组DragImageView集
     * @index 当前组在DragImageView map集合中的下标，为了获取坐标位置
     * @keyOfFileName 当前组在DragImageView map集合中的key，为了在动画结束后回调中确定当前组
     * @delayTime 当前组在动画播放前需要休眠的时间，为了达到每组都能顺序播放动画
     */
    void playMoveAnimationForGroup(cocos2d::Vector<DragImageView*> dragImageList, int index, const std::string &keyOfFileName, float delayTime, const std::function<void(std::string)> &animactionOver);
    
    /**
     * 禁止操作栏图案集操作
     */
    void forbidIconsToDrag();
    
    /**
     * 是否已经达到了最大可放组
     */
    bool isMaxGroup(const std::string &filename);

private:

    enum BTN_TAGS {
        PREV_BTN = 111,
        NEXT_BTN,
        DANCE_BTN
    };

    cocos2d::EventListenerTouchOneByOne* eventListener;
    bool isDragEnabled;
    cocos2d::Rect hotRect; //活动区域
    cocos2d::ui::Button* prevBtn;
    cocos2d::ui::Button* nextBtn;
    cocos2d::Vector<PlayImageView*> dragImageList; //操作栏拖拽图案集
    PlayImageView* seletedImage; //选中的图案
    cocos2d::Sprite* tempSprite; //当前copy的拖拽图案
    std::map<std::string, cocos2d::Vector<DragImageView*>> dragImageViewMap; //创建的拖拽图案集
    int currentPage; //当前页
    cocos2d::ui::Button* danceBtn;
    float totalSpendTime; //统计动画播放时间
    bool needSetAlpha; //是否需要在播放动画时设置半透明
};


#endif //PROJ_ANDROID_STUDIO_NUMBERINTENLAYER_H
