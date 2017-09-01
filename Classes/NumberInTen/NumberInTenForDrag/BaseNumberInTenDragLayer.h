//
// Created by Administrator on 2017/8/30.
//

#ifndef PROJ_ANDROID_STUDIO_BASENUMBERINTENDRAGLAYER_H
#define PROJ_ANDROID_STUDIO_BASENUMBERINTENDRAGLAYER_H

#include <stdio.h>
#include "BaseLayer.h"
#include "json/document.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include "ui/UIImageView.h"
#include "DragImageView.h"
#include "PlayImageView.h"
#include "PacToast.h"
#include "CommonUtils.h"

const int IconWidth = 58;
const int IconHeight = 58;

//热区编号
const int LEFT_RECT = 1;
const int RIGHT_RECT = 2;

const std::string CompareSaveFile = "NumberInTenCompare.txt";
const std::string SingularSaveFile = "NumberInTenSingular.txt";
const std::string CombineSaveFile = "NumberInTenCombine.txt";

class NumberInTenDragImpl {

public:
    
    /**
     * 初始化数据
     */
    virtual void initData() = 0;
    
    /**
     * 播放动画
     */
    virtual void playAnim() = 0;
    
    /**
     * 检测拖拽图案是否落在热区中，0表示未在热区中，1左边热区，2右边热区
     */
    virtual int checkInHotRect(cocos2d::Sprite* dragSprite) = 0;
    
    /**
     * DragImageView OnTouchEnded
     */
    virtual void onDragImageViewTouchEnded(DragImageView* dragImageView) = 0;
    
    /**
     * 判断是否已经达到了最大可放组
     */
    virtual bool isMaxGroup(const std::string &filename) = 0;
    
    /**
     * 将新增加的拖拽图片加入到集合中
     */
    virtual void addDragImageToList(DragImageView* dragImage, const std::string &filename) = 0;
    
    /**
     * 从集合中移除该删除的拖拽图片
     */
    virtual void deleteDragImageFromList(DragImageView* dragImage, const std::string &filename) = 0;
    
    /**
     * 获取指定组个数
     */
    virtual int getNumInGroup(const std::string &filename) = 0;
    
};

class BaseNumberInTenDragLayer : public BaseLayer {

public:

    BaseNumberInTenDragLayer();
    ~BaseNumberInTenDragLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();
    
    /**
     * 返回处理
     */
    virtual void onBackHandle();

    /**
     * 设置是否可拖拽
     */
    void setDragEnabled(bool isDragEnabled) {
        this->isDragEnabled = isDragEnabled;
    };
    
    /**
     * 舞动
     */
    virtual void dance();
    
    /**
     * fromJson
     */
    virtual void fromJson(const rapidjson::Document &json);
    
    /**
     * toJson
     */
    virtual void toJson(rapidjson::Document &json);

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
    
public:
    
    /**
     * 设置接口
     */
    void setNumberInTenDragImpl(NumberInTenDragImpl* numberInTenDragImpl) {
        this->numberInTenDragImpl = numberInTenDragImpl;
    }
    
    /**
     * 越界检测
     */
    void checkIsOutBorder(cocos2d::Node* node, const cocos2d::Rect &rect);
    
    /**
     * 获取最大的zOrder
     */
    int getMaxZOrder();
    
    /**
     * 播放一组移位动画
     * @dragImageList 每组DragImageView集
     * @posSX 开始X位置
     * @posSY 开始Y位置
     * @paddingH 纵向间距
     * @paddingV 横向间距
     * @keyOfFileName 当前组在DragImageView map集合中的key，为了在动画结束后回调中确定当前组
     * @delayTime 当前组在动画播放前需要休眠的时间，为了达到每组都能顺序播放动画
     * @isCrosswise 是否横向排列
     * @这组动画播放完后回调
     */
    void playMoveAnimationForGroup(cocos2d::Vector<DragImageView*> dragImageList, float posSX, float posSY, float paddingH, float paddingV, const std::string &keyOfFileName, float delayTime, bool isCrosswise, const std::function<void(std::string)> &animactionOver);
    
    /**
     * 设置操作栏图案集是否可操作
     */
    void setDragImagesEnabled(bool enabled);
    
    /**
     * 禁止除指定外的图案拖拽
     */
    void forbidIconsToDrag(const std::string &filename);
    
    /**
     * 创建拖拽图案
     */
    DragImageView* createDragImageView(const std::string &picStr, const cocos2d::Vec2 &position);

public:

    enum BTN_TAGS {
        PREV_BTN = 111,
        NEXT_BTN,
        DANCE_BTN
    };
    
    std::string saveFileName; //保存名字

    cocos2d::EventListenerTouchOneByOne* eventListener;
    bool isDragEnabled;
    cocos2d::ui::Button* prevBtn;
    cocos2d::ui::Button* nextBtn;
    cocos2d::Vector<PlayImageView*> dragImageList; //操作栏拖拽图案集
    PlayImageView* seletedImage; //选中的图案
    cocos2d::Sprite* tempSprite; //当前copy的拖拽图案
    int currentPage; //当前页
    cocos2d::ui::Button* danceBtn;
    float totalSpendTime; //统计动画播放时间
    int maxGroups; //最大可放组数
    float playAnimTime; //每个图案动画播放时间
    bool isDanced; //是否舞动
    
private:
    NumberInTenDragImpl* numberInTenDragImpl;
};


#endif //PROJ_ANDROID_STUDIO_BaseNumberInTenDragLayer_H
