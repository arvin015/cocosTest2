//
// Created by Administrator on 2019/7/4.
//

#ifndef PROJ_ANDROID_STUDIO_CLASSSCROLLVIEW_H
#define PROJ_ANDROID_STUDIO_CLASSSCROLLVIEW_H

#include "cocos2d.h"
#include "ui/UIScrollView.h"
#include "ui/UIImageView.h"

class ClassScrollView : public cocos2d::ui::ScrollView {

    typedef cocos2d::ui::ScrollView base;

public:

    static ClassScrollView* create(const cocos2d::Size &containerSize);

    ClassScrollView(const cocos2d::Size &containerSize);
    virtual ~ClassScrollView();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    void setData(const std::map<int, std::vector<std::string>> &dataMap);

    void addClass(int row, const std::string &className);

    void removeClass(int index);

private:

    void addRow(int row);

    void updateCell(int index, bool isAdd);

    cocos2d::ui::ImageView* createRowUI(const std::string &title);

    cocos2d::Node* createClassUI();

    void reload();

private:
    cocos2d::Size containerSize;

    std::map<int, std::vector<std::string>> dataMap;
    float totalY;
};


#endif //PROJ_ANDROID_STUDIO_CLASSSCROLLVIEW_H
