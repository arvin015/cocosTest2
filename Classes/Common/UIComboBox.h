//
//  UIComboBox.h
//  edxschool
//
//  Created by Arvin on 2020/3/25.
//

#ifndef UIComboBox_H
#define UIComboBox_H

#include "cocos2d.h"
#include "ui/UILayout.h"
#include "GUI/CCScrollView/CCTableView.h"

class ComboBox : public cocos2d::LayerColor,
private cocos2d::extension::TableViewDataSource,
private cocos2d::extension::TableViewDelegate {

    typedef cocos2d::LayerColor base;
    typedef ComboBox thisclass;
    
public:
    
    static ComboBox* create(const cocos2d::Size &size);
    bool init(const cocos2d::Size &size);
    
    ComboBox();
    ~ComboBox();
    
    virtual void onEnter() override;
    virtual void onExit() override;
    
    virtual void setAnchorPoint(const cocos2d::Vec2 &anchorPoint) override;
    virtual void setPosition(const cocos2d::Vec2 &position) override;
    
    void setBackgroundColor(const cocos2d::Color3B &bgColor);
    
    void addItem(const std::string &text);
    void addItemByList(const std::vector<std::string> &list);
    
    void setItemFontSize(float fontSize);
    
    void setBorderColor(const cocos2d::Color4F &borderColor);
    
    void setItemHeight(float itemHeight);
    
    void showBorder();
    
    void loadItemData();
    
    void addOnItemClickCallback(const std::function<void(int, const std::string&)> &callback);
    
private:
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    
    virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) {}
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {}
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);
    virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx);
    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table);
    
    void initView();
    
private:
    
    struct Item {
        
        Item(const std::string &text);
        
        std::string text;
    };
    
    float _fontSize;
    cocos2d::Color3B _bgColor;
    cocos2d::Color4F _borderColor;
    bool _showBorder;
    float _itemHeight;
    
    std::vector<Item> _items;
    
    cocos2d::Size _viewSize;
    cocos2d::Vec2 _anchorPoint;
    cocos2d::Vec2 _position;
    
    cocos2d::ui::Layout* bgLayout;
    cocos2d::extension::TableView* tableView;
    cocos2d::DrawNode* borderDrawNode;
    
    std::function<void(int, const std::string&)> _onItemClickCallback;
    cocos2d::EventListenerTouchOneByOne* _listener;
};

#endif /* UIComboBox_H */
