//
// Created by arvin on 2017/9/25.
//

#ifndef PROJ_ANDROID_STUDIO_LOADDIALOG_H
#define PROJ_ANDROID_STUDIO_LOADDIALOG_H

#include "cocos2d.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"

class LoadDialog : public cocos2d::Layer {

public:

    LoadDialog();
    ~LoadDialog();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(LoadDialog);

public:

    /**
     * 设置是否可通过点按屏幕关闭
     * @param cancelable
     */
    void setCancelable(bool cancelable);

    /**
     * 显示
     */
    void show(cocos2d::Node* parent);

    /**
     * 消失
     */
    void dismiss();

    /**
     * 设置显示文本
     * @param text
     */
    void setText(const std::string &text);

    /**
     * 设置文本颜色
     * @param textColor
     */
    void setTextColor(const cocos2d::Color4B &textColor);

    /**
     * 设置文本大小
     * @param textSize
     */
    void setTextSize(int textSize);

private:

    /**
     * 开始计时
     */
    void startTimer();

    /**
     * 停止计时
     */
    void stopTimer();

private:
    cocos2d::EventListenerTouchOneByOne* eventListener;
    bool cancelable;
    cocos2d::ui::Text* loadText;
    cocos2d::ui::ImageView* loadingImage;
};


#endif //PROJ_ANDROID_STUDIO_LOADDIALOG_H
