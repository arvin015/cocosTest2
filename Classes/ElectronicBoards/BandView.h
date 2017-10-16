//
//  BandView.h
//  CocosTest2
//
//  Created by arvin on 2017/10/13.
//
//

#ifndef BandView_h
#define BandView_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UIImageView.h"

class BandView : public cocos2d::Node {

public:
    
    typedef std::function<void(BandView*, float, float, bool)> OnBandTouchEndedCallback;
    
    BandView();
    ~BandView();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    CREATE_FUNC(BandView);
    
public:
    
    /**
     * 更新皮筋
     */
    void update(const cocos2d::Vec2 &curPosition);
    
    /**
     * 初始化皮筋
     */
    void initData(const cocos2d::Vec2 &startPoint, const cocos2d::Vec2 &endPoint, const cocos2d::Color4F &color);

    /**
     * 处理橡皮筋是否挂住
     */
    void checkBandIsHanged(const std::vector<cocos2d::Vec2> &pointList);

    /**
     * 设置touchEnded回调
     */
    void setOnBandTouchEndedListener(OnBandTouchEndedCallback callback) {
        this->touchEndedCallback = callback;
    }
    
private:

    /**
     * 创建首尾连接钉子
     */
    cocos2d::ui::ImageView* createSnapImage(const std::string &imageName, const cocos2d::Vec2 &position);
    
    /**
     * 检测是否选中钉子
     */
    cocos2d::ui::ImageView* checkIsSelectSnapImage(const cocos2d::Vec2 &point);

    /**
     * 检测触摸点是否在橡皮筋上
     */
    cocos2d::ui::ImageView* checkIsOnBand(const cocos2d::Vec2 &point);

    /**
     * 检测是否在线段上
     */
    bool checkIsOnSegment(const cocos2d::Vec2 &point1, const cocos2d::Vec2 &point2, const cocos2d::Vec2 &point);

    /**
     * 检测是否在三角形内
     */
    bool checkIsInTrig(const cocos2d::Vec2 &point1, const cocos2d::Vec2 &point2, const cocos2d::Vec2 &point3,
                       const cocos2d::Vec2 &point);
    
    /**
     * 置顶
     */
    void bringToFront();
    
private:
    
    cocos2d::EventListenerTouchOneByOne* eventListener;
    cocos2d::DrawNode* lineDraw;
    cocos2d::Color4F lineColor;
    cocos2d::Vector<cocos2d::ui::ImageView*> snapImageList;
    std::map<cocos2d::ui::ImageView*, cocos2d::ui::ImageView*> bandLineMap; //每段皮筋Map集合
    bool enableTouch;
    cocos2d::ui::ImageView* operateSnapImage; //当前操作的钉子
    cocos2d::ui::ImageView* keyImage; //操作依赖的橡皮筋key值钉子
    OnBandTouchEndedCallback touchEndedCallback;
    bool isUnfold; //橡皮筋是否展开
    bool isHangBand; //是否挂橡皮筋
};

#endif /* BandView_h */
