//
//  UIComboBox.cpp
//  edxschool
//
//  Created by Arvin on 2020/3/25.
//

#include "UIComboBox.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;

ComboBox* ComboBox::create(const cocos2d::Size &size)
{
    auto pRet = new (std::nothrow) ComboBox();
    if (pRet && pRet->init(size)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        return nullptr;
    }
}

bool ComboBox::init(const cocos2d::Size &size)
{
    if (!base::initWithColor(Color4B(0, 0, 0, 0))) {
        return false;
    }
    
    _viewSize = size;
    
    return true;
}

ComboBox::ComboBox() : bgLayout(nullptr)
, tableView(nullptr)
, borderDrawNode(nullptr)
, _fontSize(24)
, _bgColor(Color3B::WHITE)
, _showBorder(false)
, _borderColor(Color4F::GRAY)
, _itemHeight(50)
{}

ComboBox::~ComboBox()
{}

void ComboBox::onEnter()
{
    base::onEnter();
    
    initView();
    
    _listener = EventListenerTouchOneByOne::create();
    _listener->setSwallowTouches(true);
    _listener->onTouchBegan = CC_CALLBACK_2(thisclass::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
}

void ComboBox::onExit()
{
    base::onExit();
    _eventDispatcher->removeEventListener(_listener);
}

void ComboBox::setAnchorPoint(const Vec2 &anchorPoint)
{
    _anchorPoint = anchorPoint;
    if (bgLayout) {
        bgLayout->setAnchorPoint(_anchorPoint);
    }
}

void ComboBox::setPosition(const cocos2d::Vec2 &position)
{
    _position = position;
    if (bgLayout) {
        bgLayout->setPosition(position);
    }
}

void ComboBox::addItem(const std::string &text)
{
    _items.emplace_back(text);
}

void ComboBox::addItemByList(const std::vector<std::string> &list)
{
    for (auto str : list) {
        _items.emplace_back(str);
    }
}

void ComboBox::setItemFontSize(float fontSize)
{
    _fontSize = fontSize;
}

void ComboBox::setBackgroundColor(const Color3B &bgColor)
{
    _bgColor = bgColor;
    if (bgLayout) {
        bgLayout->setBackGroundColor(bgColor);
    }
}

void ComboBox::setBorderColor(const Color4F &borderColor)
{
    _borderColor = borderColor;
}

void ComboBox::setItemHeight(float itemHeight)
{
    _itemHeight = itemHeight;
}

void ComboBox::showBorder()
{
    _showBorder = true;
    if (borderDrawNode) {
        borderDrawNode->clear();
        borderDrawNode->drawRect(Vec2::ZERO, _viewSize, _borderColor);
    }
}

void ComboBox::loadItemData()
{
    tableView->reloadData();
}

void ComboBox::addOnItemClickCallback(const std::function<void (int, const std::string &)> &callback)
{
    _onItemClickCallback = callback;
}

bool ComboBox::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    removeFromParentAndCleanup(true);
    return true;
}

void ComboBox::tableCellTouched(TableView* table, TableViewCell* cell)
{
    if (_onItemClickCallback) {
        runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create([this, cell]{
            removeFromParentAndCleanup(true);
            
            int idx = static_cast<int>(cell->getIdx());
            _onItemClickCallback(idx, _items[idx].text);
        }), NULL));
    }
}

Size ComboBox::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return Size(tableView->getContentSize().width, _itemHeight);
}

TableViewCell* ComboBox::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto itemSize = tableCellSizeForIndex(table, idx);
    
    TableViewCell *cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
        
        auto text = Label::createWithSystemFont("", "", _fontSize);
        text->setTextColor(Color4B::BLACK);
        text->setPosition(itemSize.width * 0.5f, itemSize.height * 0.5f);
        text->setTag(0);
        cell->addChild(text);
        
        auto drawNode = DrawNode::create();
        drawNode->drawLine(Vec2(0, 0), Vec2(itemSize.width, 0), Color4F::BLACK);
        drawNode->setTag(1);
        cell->addChild(drawNode);
    }
    
    auto item = _items.at(idx);
    auto text = dynamic_cast<Label*>(cell->getChildByTag(0));
    text->setString(item.text);
    
    auto drawNode = dynamic_cast<DrawNode*>(cell->getChildByTag(1));
    drawNode->setVisible(idx < numberOfCellsInTableView(table) - 1);
    
    return cell;
}

ssize_t ComboBox::numberOfCellsInTableView(TableView *table)
{
    return _items.size();
}

void ComboBox::initView()
{
    bgLayout = Layout::create();
    bgLayout->setContentSize(_viewSize);
    bgLayout->setBackGroundColor(_bgColor);
    bgLayout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    bgLayout->setIgnoreAnchorPointForPosition(false);
    bgLayout->setAnchorPoint(_anchorPoint);
    bgLayout->setPosition(_position);
    bgLayout->setSwallowTouches(true);
    addChild(bgLayout);
    
    tableView = TableView::create(this, _viewSize);
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    tableView->setDirection(TableView::Direction::VERTICAL);
    tableView->setDelegate(this);
    bgLayout->addChild(tableView);
    
    borderDrawNode = DrawNode::create();
    bgLayout->addChild(borderDrawNode);
    if (_showBorder) {
        borderDrawNode->drawRect(Vec2::ZERO, _viewSize, _borderColor);
    }
}

ComboBox::Item::Item(const std::string &text):
text(text)
{}
