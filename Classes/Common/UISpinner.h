//
//  UISpinner.hpp
//  modelmethod
//  下拉列表
//  Created by Alex on 3/1/2017.
//
//

#ifndef _UI_SPINNER_H_
#define _UI_SPINNER_H_

#include "ui/UILayout.h"
#include "UIMathNumber.h"

/**
 * 選擇下拉選項時的回調
 */
class OnSpinnerSelectedListener
{
    public :
    
    // 點擊下拉選項時的回調
    virtual void onItemSelected(int index, const std::string &content) = 0;
    
    virtual ~OnSpinnerSelectedListener(){};
};

class Spinner :

public cocos2d::ui::Layout

{
    
public:
    
    Spinner();
    ~Spinner();
    
    CREATE_FUNC(Spinner);
    
    bool init();
    
    virtual void onEnter();
    virtual void onExit();
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    /**
     * 設置當點擊Spinner外部區域時，Spinner是否隱藏
     * @param cancel true，取消，false，不取消
     */
    void setCanceledOnTouchOutside(bool cancel);
    
    /*
     * 設置下拉選項在不同狀態時的顏色
     * @param normal 正常狀態
     * @param selected 點擊狀態
     */
    void setItemBackgroundColor(const cocos2d::Color3B &normal, const cocos2d::Color3B &selected);
    
    /**
     * 設置選項分割線的顏色
     */
    void setItemDividerColor(const cocos2d::Color3B & color);
    
    /**
     * 設置下拉選項字體的顏色
     */
    void setItemFontColor(const cocos2d::Color4B & color);
    
    /**
     * 設置下拉選項字體的大小
     */
    void setItemFontSize(float fontSize);
    
    /**
     * 設置下拉列表的參數
     * @param list 下拉列表中填充的文字數據
     * @param itemWidth 下拉選項的寬度
     * @param itemHeight 下拉選項的高度
     * @param showDivider 是否顯示分隔線
     */
    void setParameters(const std::vector<std::string> &list, float itemWidth, float itemHeight, bool showDivider = true);
    
    /**
     * 設置下拉列表的參數---可支持分數
     * @param list 下拉列表中填充的文字數據
     * @param itemWidth 下拉選項的寬度
     * @param itemHeight 下拉選項的高度
     * @param showDivider 是否顯示分隔線
     */
    void setParameters2(const std::vector<std::string> &list, float itemWidth, float itemHeight, bool showDivider = true);
    
    void setOnSpinnerSelectedListener(OnSpinnerSelectedListener *listener);
    
    void onItemTouch(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
    
    /**
     * 取得選擇的Spinner中選項的索引
     * @return -1表示未有選項被選中
     */
    int getSelectedItemIndex();
    void setSelectedItemIndex(int index);
    
    /**
     * 顯示或隱藏邊框
     */
    void showBorder();
    void hideBorder();
    
private:
    
    cocos2d::Color3B _itemBgColorNormal;// item正常狀態背景顏色
    cocos2d::Color3B _itemBgColorSelected;// item選中狀態背景顏色
    cocos2d::Color3B _itemDividerColor;// item分割線的顏色
    cocos2d::Color4B _itemFontColor;// item字體顏色
    
    float _itemFontSize;// item字體大小
    int _itemSelected;// 被選中的item的位置，由0開始編號
    
    bool _autoHide;// 觸摸Spinner區域之外是否要自動隱藏Spinner
    
    std::vector<cocos2d::ui::Layout*> _items;// 實現item的Layout集合
    
    OnSpinnerSelectedListener* _listener;// 選中item時的回調
    cocos2d::EventListenerTouchOneByOne* _touch;// 觸摸事件
    
    cocos2d::DrawNode* _drawNode;// 繪製Spinner邊框
};

#endif /* _UI_SPINNER_H_ */
