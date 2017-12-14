//
//  PolygonView.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/13.
//

#ifndef PolygonView_h
#define PolygonView_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "PolygonSegment.h"

class PolygonView : public cocos2d::Node {

public:

    typedef std::function<void(std::vector<PolygonPoint>)> OnCutCallback;

    enum OperateType {
        CUT,
        MOVE
    };

    PolygonView();
    virtual ~PolygonView();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(PolygonView);

    /**
     * 设置值
     */
    void setValue(std::vector<PolygonPoint> pointList);

    /**
     * 设置是否可Touch
     */
    void setIsTouchEnabled(bool isTouchEnabled) {
        this->isTouchEnabled = isTouchEnabled;
    }

    /**
     * 设置操作类型
     */
    void setOperateType(OperateType operateType) {
        this->operateType = operateType;
    }

    /**
     * 设置切割回调
     */
    void setOnCutCallback(OnCutCallback cutcallback) {
        this->cutcallback = cutcallback;
    }

private:

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    /**
     * 绘制多边形
     */
    void onDrawPolygon();

    /**
     * 绘制切割线
     */
    void onDrawLine();

    /**
     * Touch是否在多边形内
     */
    bool checkIsInPolygon(const cocos2d::Vec2 &point);

    /**
     * 检测切割相交点
     */
    std::map<PolygonPoint, PolygonPoint> checkIntersectPoints(const cocos2d::Vec2 &p1, const cocos2d::Vec2 &p2);

    /**
     * 添加分割点
     */
    void addCrossPoints(std::map<PolygonPoint, PolygonPoint> pointMap);

    /**
     * 切割操作
     */
    void cutPolygon();

private:

    cocos2d::EventListenerTouchOneByOne* eventListener;
    cocos2d::DrawNode* polygonDrawNode;
    cocos2d::DrawNode* lineDrawNode;

    std::vector<PolygonPoint> pointList;
    std::vector<PolygonSegment> segmentList;

    bool isTouchEnabled;
    bool isSelect;

    OperateType operateType;

    cocos2d::Vec2 firstPosition; //第一个点位置
    cocos2d::Vec2 curPosition; //当前位置

    OnCutCallback cutcallback;

};

#endif /* PolygonView_h */
