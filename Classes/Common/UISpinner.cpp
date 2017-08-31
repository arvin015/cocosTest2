//
//  UISpinner.cpp
//  modelmethod
//
//  Created by Alex on 3/1/2017.
//
//

#include "ui/UIButton.h"
#include "ui/UIText.h"

#include "UISpinner.h"

USING_NS_CC;
using namespace cocos2d::ui;

Spinner::Spinner()
{
    _itemBgColorNormal = Color3B::WHITE;
    _itemBgColorSelected = Color3B::YELLOW;
    _itemDividerColor = Color3B::GRAY;
    _itemFontColor = Color4B::BLACK;
    
    _itemFontSize = 24.0f;
    _itemSelected = -1;
    
    _autoHide = true;
    
    _listener = nullptr;
    _touch = nullptr;
    
    _drawNode = nullptr;
}

Spinner::~Spinner()
{}

bool Spinner::init()
{
    if (!Layout::init()) {
        return false;
    }
    
    setLayoutType(Layout::Type::VERTICAL);
    
    return true;
}

void Spinner::onEnter()
{
    Layout::onEnter();
    
    if (nullptr == _touch) {
        _touch = EventListenerTouchOneByOne::create();
        _touch->setSwallowTouches(false);
        _touch->onTouchBegan = CC_CALLBACK_2(Spinner::onTouchBegan, this);
        _touch->onTouchMoved = CC_CALLBACK_2(Spinner::onTouchMoved, this);
        _touch->onTouchEnded = CC_CALLBACK_2(Spinner::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(_touch, this);
    }
}

void Spinner::onExit()
{
    if (nullptr != _touch) {
        _eventDispatcher->removeEventListener(_touch);
    }
    
    Layout::onExit();
}

bool Spinner::onTouchBegan(Touch *touch, Event *event)
{
    return true;
}

void Spinner::onTouchMoved(Touch *touch, Event *event)
{}

void Spinner::onTouchEnded(Touch *touch, Event *event)
{
    Point touchLocation = convertTouchToNodeSpace(touch);
    if (_autoHide && !getBoundingBox().containsPoint(touchLocation)) {
        setVisible(false);
    }
}

void Spinner::setCanceledOnTouchOutside(bool cancel)
{
    _autoHide = cancel;
}

void Spinner::setItemBackgroundColor(const cocos2d::Color3B &normal, const cocos2d::Color3B &selected)
{
    _itemBgColorNormal = normal;
    _itemBgColorSelected = selected;
}

void Spinner::setItemDividerColor(const cocos2d::Color3B &color)
{
    _itemDividerColor = color;
}

void Spinner::setItemFontColor(const cocos2d::Color4B &color)
{
    _itemFontColor = color;
}

void Spinner::setItemFontSize(float fontSize)
{
    _itemFontSize = fontSize;
}

void Spinner::setParameters(const std::vector<std::string> &list, float itemWidth, float itemHeight, bool showDivider)
{
    const float dividerHeight = 1.0f;
 
    _items.clear();
    for (int i = 0; i < list.size(); ++i) {
        
        Layout* bgLayout = Layout::create();
        bgLayout->setLayoutType(Type::RELATIVE);
        bgLayout->setBackGroundColorType(BackGroundColorType::SOLID);
        bgLayout->setBackGroundColor(_itemBgColorNormal);
        bgLayout->setContentSize(Size(itemWidth, itemHeight));
        bgLayout->setTouchEnabled(true);
        bgLayout->addTouchEventListener(CC_CALLBACK_2(Spinner::onItemTouch, this));
        bgLayout->setTag(i);
        addChild(bgLayout);
        _items.push_back(bgLayout);
        
        Text* text = Text::create();
        text->setFontSize(_itemFontSize);
        text->setTextColor(_itemFontColor);
        text->setString(list[i]);
        text->setTag(0);
        bgLayout->addChild(text);
        
        RelativeLayoutParameter* lp = RelativeLayoutParameter::create();
        lp->setAlign(RelativeLayoutParameter::RelativeAlign::CENTER_IN_PARENT);
        text->setLayoutParameter(lp);
        
        // 添加分隔線
        if (showDivider && i < list.size() - 1) {
            Layout* divider = Layout::create();
            divider->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
            divider->setBackGroundColor(_itemDividerColor);
            divider->setContentSize(Size(itemWidth, dividerHeight));
            addChild(divider);
        }
    }
    
    setContentSize(Size(itemWidth, (itemHeight + dividerHeight) * list.size() - dividerHeight));
}

void Spinner::setParameters2(const std::vector<std::string> &list, float itemWidth, float itemHeight, bool showDivider)
{
    const float dividerHeight = 1.0f;
    
    _items.clear();
    for (int i = 0; i < list.size(); ++i) {
        
        Layout* bgLayout = Layout::create();
        bgLayout->setLayoutType(Type::RELATIVE);
        bgLayout->setBackGroundColorType(BackGroundColorType::SOLID);
        bgLayout->setBackGroundColor(_itemBgColorNormal);
        bgLayout->setContentSize(Size(itemWidth, itemHeight));
        bgLayout->setTouchEnabled(true);
        bgLayout->addTouchEventListener(CC_CALLBACK_2(Spinner::onItemTouch, this));
        bgLayout->setTag(i);
        addChild(bgLayout);
        _items.push_back(bgLayout);
        
        MathNumber* text = MathNumber::create();
        text->setFontSize(_itemFontSize);
        text->setFontColor(_itemFontColor);
        text->setString(list[i]);
        text->setTag(0);
        bgLayout->addChild(text);
        
        RelativeLayoutParameter* lp = RelativeLayoutParameter::create();
        lp->setAlign(RelativeLayoutParameter::RelativeAlign::CENTER_IN_PARENT);
        text->setLayoutParameter(lp);
        
        // 添加分隔線
        if (showDivider && i < list.size() - 1) {
            Layout* divider = Layout::create();
            divider->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
            divider->setBackGroundColor(_itemDividerColor);
            divider->setContentSize(Size(itemWidth, dividerHeight));
            addChild(divider);
        }
    }
    
    setContentSize(Size(itemWidth, (itemHeight + dividerHeight) * list.size() - dividerHeight));
}

void Spinner::setOnSpinnerSelectedListener(OnSpinnerSelectedListener* listener)
{
    _listener = listener;
}

void Spinner::onItemTouch(Ref *pSender, Widget::TouchEventType type)
{
    Layout* layout = dynamic_cast<Layout*>(pSender);
    _itemSelected = layout->getTag();
    
    if (Widget::TouchEventType::BEGAN == type) {
        
        // 被選中個的item設置選中顏色
        layout->setBackGroundColor(_itemBgColorSelected);
        
        // 未被選中的item恢復默認顏色
        for (int i = 0; i < _items.size(); ++i) {
            if (_itemSelected != _items[i]->getTag()) {
                _items[i]->setBackGroundColor(_itemBgColorNormal);
            }
        }
    } else if (Widget::TouchEventType::CANCELED == type) {
        // 取消選中時，已選中的item恢復默認顏色
        _items[_itemSelected]->setBackGroundColor(_itemBgColorNormal);
        _itemSelected = -1;
    } else if (Widget::TouchEventType::ENDED == type) {
        layout->setBackGroundColor(_itemBgColorNormal);
        
        std::string content = "";
        Text* text = dynamic_cast<Text*>(layout->getChildByTag(0));
        if(text) {
            content = text->getString();
        } else {
            MathNumber* text = dynamic_cast<MathNumber*>(layout->getChildByTag(0));
            if(text) {
                content = text->getString();
            }
        }
        
        if (nullptr != _listener) {
            _listener->onItemSelected(_itemSelected, content);
        }
        
        setVisible(false);
    }
}

int Spinner::getSelectedItemIndex()
{
    return _itemSelected;
}

void Spinner::setSelectedItemIndex(int index)
{
    _itemSelected = index;
//    _items[_itemSelected]->setBackGroundColor(_itemBgColorSelected);
}

void Spinner::showBorder()
{
    if(_drawNode == nullptr) {
        _drawNode = DrawNode::create();
        addChild(_drawNode, 99);
    }
    
    _drawNode->clear();
    _drawNode->drawRect(Vec2::ZERO, Vec2(getContentSize().width, getContentSize().height), Color4F::BLACK);
}

void Spinner::hideBorder()
{
    if(_drawNode) {
        _drawNode->clear();
    }
}
