//
//  UINumberSelector.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/11/23.
//

#ifndef UINumberSelector_h
#define UINumberSelector_h

#include <stdio.h>
#include "ui/CocosGUI.h"
#include "cocos2d.h"

class NumberSelector : public cocos2d::Node {

public:

    typedef std::function<void(int index, std::string)> OnSelectCallback;

    NumberSelector();
    virtual ~NumberSelector();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(NumberSelector);

    /**
     * 设置数据
     */
    void setValues(std::vector<std::string> valueList);

    /**
     * 设置显示Item个数
     */
    void setShowItemNum(int showItemNum) {
        this->showItemNum = showItemNum;
    }

    /**
     * 设置初始选中Item
     */
    void setSelectItem(int selectItem);

    /**
     * 设置背景颜色
     */
    void setBgColor(const cocos2d::Color3B &bgColor) {
        this->bgColor = bgColor;
    }

    /**
     * 设置背景透明度
     */
    void setBgOpacity(int bgOpacity) {
        this->bgOpacity = bgOpacity;
    }

    /**
     * 设置选中回调
     */
    void setOnSelectCallback(OnSelectCallback selectCallback) {
        this->selectCallback = selectCallback;
    }

private:

    cocos2d::ui::ListView* listView;

    std::vector<std::string> valueList;

    int showItemNum; //显示Item个数
    int selectItem; //初始选中Item
    cocos2d::Color3B bgColor; //背景颜色
    int bgOpacity; //背景透明度
    std::string bgImageName; //背景图片
    int textSize; //文本大小
    cocos2d::Color4B textColor; //文本颜色

    OnSelectCallback selectCallback;
};

#endif /* UINumberSelector_h */
