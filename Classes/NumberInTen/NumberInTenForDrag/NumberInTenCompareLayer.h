//
//  NumberInTenCompareLayer.h
//  CocosTest2
//
//  Created by arvin on 2017/8/30.
//
//

#ifndef NumberInTenCompareLayer_h
#define NumberInTenCompareLayer_h

#include <stdio.h>
#include "BaseNumberInTenDragLayer.h"

class NumberInTenCompareLayer
: public BaseNumberInTenDragLayer
, NumberInTenDragImpl {
  
public:
    
    NumberInTenCompareLayer();
    ~NumberInTenCompareLayer();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(NumberInTenCompareLayer);
    
public:
    
    /**
     * 初始化数据
     */
    virtual void initData();
    
    /**
     * 播放动画
     */
    virtual void playAnim();
    
    /**
     * 检测拖拽图案是否落在热区中
     */
    virtual int checkInHotRect(cocos2d::Sprite* dragSprite);
    
    /**
     * DragImageView OnTouchEnded
     */
    virtual void onDragImageViewTouchEnded(DragImageView* dragImageView);
    
    /**
     * 判断是否已经达到了最大可放组
     */
    virtual bool isMaxGroup(const std::string &filename);
    
    /**
     * 将新增加的拖拽图片加入到集合中
     */
    virtual void addDragImageToList(DragImageView* dragImage, const std::string &filename);
    
    /**
     * 从集合中移除该删除的拖拽图片
     */
    virtual void deleteDragImageFromList(DragImageView* dragImage, const std::string &filename);
    
    /**
     * 获取指定组个数
     */
    virtual int getNumInGroup(const std::string &filename);
    
private:
    cocos2d::Rect hotRect; //接收拖拽图片热区
    std::map<std::string, cocos2d::Vector<DragImageView*>> dragImageViewMap; //创建的拖拽图案集
};

#endif /* NumberInTenCompareLayer_h */
