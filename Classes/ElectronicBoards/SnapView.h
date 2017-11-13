//
//  SnapView.hpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/11/13.
//

#ifndef SnapView_h
#define SnapView_h

#include <stdio.h>
#include "cocos2d.h"

class SnapView : public cocos2d::Sprite {

public:

    typedef std::function<void()> OnTouchDown;
    typedef std::function<void()> OnTouchMove;
    typedef std::function<void(float, float)> OnTouchEnd;

    SnapView();
    virtual ~SnapView();
    
    virtual void onEnter();
    virtual void onExit();

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    virtual bool init();
    
    CREATE_FUNC(SnapView);

    static SnapView* create(const std::string &imageName);
    bool init(const std::string &imageName);

    void setP(int row, int col) {
        this->row = row;
        this->col = col;
    }

    int getRow() {
        return row;
    }

    int getCol() {
        return col;
    }

    void setOnTouchDown(const OnTouchDown &touchDown) {
        this->touchDown = touchDown;
    }

    void setOnTouchMove(const OnTouchMove &touchMove) {
        this->touchMove = touchMove;
    }

    void setOnTouchEnd(const OnTouchEnd &touchEnd) {
        this->touchEnd = touchEnd;
    }

public:

    SnapView* preSnapView;
    SnapView* nextSnapView;

private:

    cocos2d::EventListenerTouchOneByOne* eventListener;
    int row;
    int col;
    std::string imageName;

    bool isInArea;

    OnTouchDown touchDown;
    OnTouchMove touchMove;
    OnTouchEnd touchEnd;
};

#endif /* SnapView_h */
