/*
 * UICommonTableView.cpp
 *
 *  Created on: 2017年8月18日
 *      Author: arvin.li
 */

#include "UICommonTableView.h"
#include "json/document.h"

using namespace cocos2d::ui;
using namespace rapidjson;

USING_NS_CC;
USING_NS_CC_EXT;

CommonTableView::CommonTableView()
: _tableView(nullptr)
, _selectGridColumn(-1)
, _selectGridIndex(-1)
, _isGridClicked(false) {
}

CommonTableView::~CommonTableView() {
}

bool CommonTableView::init()
{
    if (!Node::init()) {
        return false;
    }
    
    return true;
}

void CommonTableView::onEnter()
{
    Node::onEnter();
    
    if(!_tableView){
        _tableView = TableView::create(this, Size(GRID_VIEW_WIDTH, GRID_VIEW_HEIGHT));
        _tableView->setDirection(TableView::Direction::VERTICAL);
        _tableView->setDelegate(this);
        _tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
        addChild(_tableView);
        _tableView->reloadData();
    }
}

void CommonTableView::onExit()
{
    Node::onExit();
}

void CommonTableView::cleanup() {
    Director::getInstance()->getTextureCache()->removeUnusedTextures();
}

void CommonTableView::tableCellTouched(TableView* table, TableViewCell* cell)
{
    if (_isGridClicked) { // 確保點擊的是Item，而非只是cell
        int lastSelectCellIndex = _selectGridIndex / GRID_VIEW_COLUMN; //获取上一个选中的cell
        _selectGridIndex = (int)cell->getIdx() * GRID_VIEW_COLUMN + _selectGridColumn; //获取当前选中的grid index
        table->updateCellAtIndex(lastSelectCellIndex);
        table->updateCellAtIndex(cell->getIdx());
        _isGridClicked = false;
        
        onSelectIndex(_selectGridIndex); // 交給子類處理
        
        log("tableCellTouched when click Grid. cell #%zd, grid #%d, index #%d", cell->getIdx(), _selectGridColumn, _selectGridIndex);
        
    } else {
        log("tableCellTouched when not click Grid.");
    }
}

Size CommonTableView::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    // 最後一行不應包含豎直間距（GRID_ITEM_SPACE_VERTICAL）
    int itemHeight = GRID_ITEM_HEIGHT + GRID_ITEM_SPACE_VERTICAL;
    if (idx + 1 == numberOfCellsInTableView(table)) {
        itemHeight = GRID_ITEM_HEIGHT;
    }
    
    return Size(GRID_VIEW_WIDTH, itemHeight);
}

TableViewCell* CommonTableView::tableCellAtIndex(TableView *table, ssize_t idx)
{
    TableViewCell *cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
        
        for(int i = 0; i < GRID_VIEW_COLUMN; ++i) {
            Layout* root = Layout::create();
            root->setLayoutType(Layout::Type::RELATIVE);
            root->setBackGroundImageScale9Enabled(true);
            root->setBackGroundImageCapInsets(Rect(15, 10, 18, 7));
            root->setContentSize(Size(GRID_ITEM_WIDTH, GRID_ITEM_HEIGHT));
            root->addClickEventListener(CC_CALLBACK_1(CommonTableView::onGridItemClicked, this));
            root->setTouchEnabled(true);
            root->setSwallowTouches(false);
            root->setTag(i);
            cell->addChild(root);
            
            if(_impl) {
                _impl->layoutItem(root); // 交給子類處理
            }
        }
    }
    
    long itemAboveCount = idx * GRID_VIEW_COLUMN;// 當前行之前的所有Item總數
    bool hideStuLeft = false;// 是否隱藏本行剩餘的Item
    
    // fill data
    for(int i = 0; i < GRID_VIEW_COLUMN; ++i) {
        Layout* root = (Layout*) cell->getChildByTag(i);
        
        long index = idx * GRID_VIEW_COLUMN + i;
        
        // 最後一行不應包含豎直間距（GRID_ITEM_SPACE_VERTICAL）
        if (idx + 1 == numberOfCellsInTableView(table)) {
            root->setPosition(Vec2(i * (GRID_ITEM_WIDTH + GRID_ITEM_SPACE_HORIZONTAL), 0));
        } else {
            root->setPosition(Vec2(i * (GRID_ITEM_WIDTH + GRID_ITEM_SPACE_HORIZONTAL), GRID_ITEM_SPACE_VERTICAL));
        }
        
        bool isSelected = false;
        
        // 獲取是否選中
        if (-1 < _selectGridIndex && _selectGridIndex == itemAboveCount + i) {
            isSelected = true;
        } else {
            isSelected = false;
        }
        
        /**
         * 最後一行Item數量小於GRID_VIEW_COLUMN的情況下
         * 隱藏剩餘部分的Item
         */
        if (itemAboveCount + i + 1 > COUNTS) {
            hideStuLeft = true;
        }
        
        root->setVisible(!hideStuLeft);
        
        // 設置Item數據
        if (!hideStuLeft) {
            if(_impl) {
                _impl->setItemData(root, index, isSelected);
            }
        }
    }
    
    return cell;
}

ssize_t CommonTableView::numberOfCellsInTableView(TableView *table)
{
    ssize_t row = COUNTS / GRID_VIEW_COLUMN;
    if (0 < COUNTS % GRID_VIEW_COLUMN) {
        row++;
    }
    return row;
}

void CommonTableView::onGridItemClicked(Ref* pSender)
{
    // 計算選中的列
    Node* node = static_cast<Node*> (pSender);
    _selectGridColumn = node->getTag();
    _isGridClicked = true;
}

void CommonTableView::setBasicData(int itemW, int itemH, int spaceH, int spaceV, int column) {
   
    GRID_ITEM_WIDTH = itemW;
    GRID_ITEM_HEIGHT = itemH;
    GRID_ITEM_SPACE_HORIZONTAL = spaceH;
    GRID_ITEM_SPACE_VERTICAL = spaceV;
    GRID_VIEW_COLUMN = column;
}

void CommonTableView::setItemCounts(int counts) {
    COUNTS = counts;
    
    // 重置數據
    _selectGridColumn = -1;
    _selectGridIndex = -1;
    _isGridClicked = false;
    
    if(_tableView) {
        _tableView->reloadData();
    }
}
