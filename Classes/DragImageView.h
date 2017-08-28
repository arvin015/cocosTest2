//
// Created by Administrator on 2017/8/27.
//

#ifndef PROJ_ANDROID_STUDIO_DRAGIMAGEVIEW_H
#define PROJ_ANDROID_STUDIO_DRAGIMAGEVIEW_H

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UIImageView.h"

class DragImageView;
typedef std::function<void(DragImageView*)> OnMoveEndedCallback;

class DragImageView : public cocos2d::ui::ImageView {

public:

    DragImageView();
    ~DragImageView();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();
    bool init(const std::string& imageFileName, TextureResType texType = TextureResType::LOCAL);

    CREATE_FUNC(DragImageView);

    static DragImageView* create(const std::string &filename);

    /**
     * 设置OnMoveEndedCallback
     */
    void setOnMoveEndedCallback(OnMoveEndedCallback moveEndedCallback) {
        this->moveEndedCallback = moveEndedCallback;
    }

private:

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

private:
    cocos2d::EventListenerTouchOneByOne* eventListener;
    bool isMoveEnabled; //是否可移动
    bool isMoving; //是否正在移动
    OnMoveEndedCallback moveEndedCallback;
};


#endif //PROJ_ANDROID_STUDIO_DRAGSPRITE_H
