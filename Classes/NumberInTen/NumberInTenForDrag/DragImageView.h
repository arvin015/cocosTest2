//
// Created by Administrator on 2017/8/27.
//

#ifndef PROJ_ANDROID_STUDIO_DRAGIMAGEVIEW_H
#define PROJ_ANDROID_STUDIO_DRAGIMAGEVIEW_H

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "ui/UIText.h"
#include "json/document.h"

typedef std::function<void()> OnMoveEndedCallback;
typedef std::function<void()> OnDeleteCallback;

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
    
    /**
     * 设置OnDeleteCallback
     */
    void setOnDeleteCallback(OnDeleteCallback deleteCallback) {
        this->deleteCallback = deleteCallback;
    }
    
    /**
     * 删除按钮显示隐藏
     */
    void setDeleteBtnVisible(bool visible);
    
    /**
     * 添加显示数字
     */
    void addShowNum(int showNum);
    
    /**
     * 添加下方统计数字
     */
    void addShowCountNum(int countNum, int opacity = 255);
    
    /**
     * 获取显示数字
     */
    int getShowNum();
    
    /**
     * 设置是否半透明
     */
    void setIsTranslucent(bool isTranslucent);
    
    /**
     * 设置是否可移动
     */
    void setIsMoveEnabled(bool isMoveEnabled) {
        this->isMoveEnabled = isMoveEnabled;
    }
    
    /**
     * 将图案置顶
     */
    void bringToFront();
    
    /**
     * 获取统计文本
     */
    cocos2d::ui::Text* getCountText() {
        return showCountText;
    }
    
    /**
     * fromJson
     */
    void fromJson(const rapidjson::Value &json);
    
    /**
     * toJson
     */
    void toJson(rapidjson::Document &obj);

private:

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

private:
    cocos2d::ui::Button* deleteBtn; //删除按钮
    cocos2d::ui::Text* showNumText; //显示数字文本
    cocos2d::ui::Text* showCountText; //显示下方统计文本
    cocos2d::EventListenerTouchOneByOne* eventListener;
    bool isMoveEnabled; //是否可移动
    bool isSelected; //是否选中
    bool isMove; //是否移动
    OnMoveEndedCallback moveEndedCallback;
    OnDeleteCallback deleteCallback;
};


#endif //PROJ_ANDROID_STUDIO_DRAGSPRITE_H
