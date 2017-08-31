//
//  FlyView.h
//  CocosTest2
//
//  Created by arvin on 2017/8/31.
//
//

#ifndef FlyView_h
#define FlyView_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIButton.h"
#include "ui/UIImageView.h"
#include "UISpinner.h"

//每个格子的宽高
const int FLY_GRID_WIDTH = 60;
const int FLY_GRID_HEIGHT = 60;

class FlyView : public cocos2d::ui::Layout, OnSpinnerSelectedListener {

public:
    
    FlyView();
    ~FlyView();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(FlyView);
    
    /**
     * 点击数字处理
     */
    void onClickNumHandle(int num);
    
    /**
     * 点击下拉选项回调
     */
    virtual void onItemSelected(int index, const std::string &content);
    
    /**
     * 设置数字是否可点击
     */
    void setClickEnabled(bool clickEnabled);
    
    /**
     * 舞动
     */
    void dance();
    
private:
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    /**
     * 数字格子变色
     */
    void updateColor(int num);
    
    /**
     * 创建指向箭头
     */
    void addDirectArrow(int beginNum, int endNum);
    
    /**
     * 判断起始点是否都存在
     */
    bool existB_E();

private:
    cocos2d::EventListenerTouchOneByOne* eventListener;
    cocos2d::ui::Button* deleteBtn; //删除按钮
    cocos2d::Sprite* beeSprite; //飞行蜜蜂
    cocos2d::ui::Layout* tipView; //提示
    bool isSelect; //是否选中
    bool isMove; //是否移动
    Spinner* spinner; //弹出框
    cocos2d::ui::Button* beginBtn; //起点Button
    cocos2d::ui::Button* endBtn; //终点Button
    cocos2d::ui::ImageView* arrowImage; //箭头
    int selectNum; //当前选中的数字
    bool isClickEnabled; //数字是否可点击
};

#endif /* FlyView_h */
