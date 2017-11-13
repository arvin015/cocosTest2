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
#include "SnapView.h"

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
     * 拖拽点放手后更新
     */
    void updateAfterTouchEnd(const cocos2d::Vec2 &point);

    /**
     * 更新橡皮筋
     */
    void updateBands();
    
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
    
    /**
     * 检测是否在三角形内
     */
    bool checkIsInTrig(const cocos2d::Vec2 &point);
    
private:

    /**
     * 创建首尾连接钉子
     */
    SnapView* createSnapView(const std::string &imageName, const cocos2d::Vec2 &position);

    /**
     * 检测触摸点是否在橡皮筋上
     */
    SnapView* checkIsOnBand(const cocos2d::Vec2 &point);

    /**
     * 检测是否在线段上
     */
    bool checkIsOnSegment(const cocos2d::Vec2 &point1, const cocos2d::Vec2 &point2, const cocos2d::Vec2 &point);
    
    /**
     * 检测是否在三角形内
     */
    bool checkPointIsInTrig(const cocos2d::Vec2 &point1, const cocos2d::Vec2 &point2, const cocos2d::Vec2 &point3, const cocos2d::Vec2 &point);
    
    /**
     * 检测钉子是否满足被挂住条件
     */
    std::vector<cocos2d::Vec2> checkBandForHangIsOk(const std::vector<cocos2d::Vec2> &pointList);
    
    /**
     * 点到直线的距离
     */
    float getPoint2LineDistance(const cocos2d::Vec2 &point1, const cocos2d::Vec2 &point2, const cocos2d::Vec2 &point);

    /**
    * 置顶
    */
    void bringToFront() {
        cocos2d::Vector<cocos2d::Node*> childNodes = getParent()->getChildren();
        int localZOrder = childNodes.at(childNodes.size() - 1)->getTag();
        getParent()->reorderChild(this, localZOrder + 1);
    }
    
private:
    
    cocos2d::EventListenerTouchOneByOne* eventListener;
    cocos2d::DrawNode* lineDraw;
    cocos2d::Color4F lineColor;
    bool enableTouch;
    SnapView* operateSnapView; //当前操作的钉子
    OnBandTouchEndedCallback touchEndedCallback;
    std::vector<cocos2d::Vec2> trigPoints; //三角形三个点
    SnapView* firstSnapView;
};

#endif /* BandView_h */
