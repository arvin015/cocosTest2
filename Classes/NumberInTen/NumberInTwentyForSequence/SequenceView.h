//
//  SequenceView.h
//  CocosTest2
//
//  Created by arvin on 2017/9/27.
//
//

#ifndef SequenceView_h
#define SequenceView_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIButton.h"
#include "ui/UIImageView.h"
#include "ui/UIText.h"
#include "UISpinner.h"
#include "json/document.h"

//每个对象的宽高
const int SEQUENCE_GRID_WIDTH = 32;
const int SEQUENCE_GRID_HEIGHT = 46;

class SequenceView : public cocos2d::ui::Layout, OnSpinnerSelectedListener {
    
public:
    
    SequenceView();
    ~SequenceView();
    
    virtual void onEnter();
    virtual void onExit();
    
    bool init(int totalNum, int targetNum);
    
    static SequenceView* create(int totalNum, int targetNum);
    
    /**
     * 点击对象处理
     */
    void onClickObjHandle(int num);
    
    /**
     * 点击下拉选项回调
     */
    virtual void onItemSelected(int index, const std::string &content);
    
    /**
     * 设置对象是否可点击
     */
    void setClickObjEnabled(bool clickEnabled);
    
    /**
     * 检查是否可舞动
     */
    bool checkCanDance();
    
    /**
     * 舞动
     */
    void dance();
    
    /**
     * fromJson
     */
    void fromJson(const rapidjson::Value &json);
    
    /**
     * toJson
     */
    void toJson(rapidjson::Document &json);
    
private:
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
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
    cocos2d::Sprite* arrowSprite; //指向箭头
    cocos2d::ui::Layout* tipView; //提示
    bool isSelect; //是否选中
    bool isMove; //是否移动
    Spinner* spinner; //弹出框
    cocos2d::ui::Button* beginBtn; //起点Button
    cocos2d::ui::Button* endBtn; //终点Button
    cocos2d::ui::ImageView* arrowImage; //箭头
    int selectNum; //当前选中的数字
    bool isClickEnabled; //数字是否可点击
    int totalNum; //总个数
    int targetNum; //目标数
    cocos2d::Vector<cocos2d::Sprite*> childList; //对象集合
    cocos2d::Vector<cocos2d::ui::Text*> numList; //数字文本集合
    std::vector<int> rndNumbers; //一组随机数
};

#endif /* SequenceView_h */
