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
using namespace ui;
using namespace std;

#define Espinon 0.000001

CommonTableView::CommonTableView()
: _tableView(nullptr)
, _selectGridNum(-1)
, _selectGridIndex(-1)
, _isGridClicked(false)
, _showDirectArrow(false)
, _topArrow(nullptr)
, _bottomArrow(nullptr)
, _impl(nullptr) {
}

CommonTableView::~CommonTableView() {
}

bool CommonTableView::init() {
    if (!Node::init()) {
        return false;
    }
    
    return true;
}

void CommonTableView::onEnter() {
    Node::onEnter();
    
    if (!_tableView){
        _tableView = TableView::create(this, Size(_gridWidth, _gridHeight));
        _tableView->setDirection(_direction == VERTICAL ? TableView::Direction::VERTICAL : TableView::Direction::HORIZONTAL);
        _tableView->setDelegate(this);
        _tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
        addChild(_tableView);

        if (isAdapterByGrid) { //父Node的Size保持与_tableView大小一致
            setContentSize(Size(_gridWidth, _gridHeight));
            setPosition(getPosition());
        }
    }
}

void CommonTableView::onExit() {
    Node::onExit();
}

void CommonTableView::cleanup() {
    Director::getInstance()->getTextureCache()->removeUnusedTextures();
}

void CommonTableView::scrollViewDidScroll(ScrollView* view) {
    ShowState showState = SHOW_ALL;
    
    if (!view->isDragging()) {
        
        float curOffsetY = view->getContentOffset().y;
        float minOffsetY = view->minContainerOffset().y;
        
        if (minOffsetY >= 0) {
            showState = SHOW_ALL;
        } else if (curOffsetY >= 0) {
            showState = SHOW_BOTTOM;
        } else if (fabs(curOffsetY - minOffsetY) < Espinon || curOffsetY < minOffsetY) {
            showState = SHOW_TOP;
        } else if (curOffsetY > minOffsetY) {
            showState = SHOW_CENTER;
        }

        if (_showDirectArrow) {
            switch (showState) {
                case SHOW_ALL: {
                    setBottomDirectArrowVisible(false);
                    setTopDirectArrowVisible(false);
                    break;
                }
                case SHOW_TOP: {
                    setBottomDirectArrowVisible(true);
                    setTopDirectArrowVisible(false);
                    break;
                }
                case SHOW_CENTER: {
                    setBottomDirectArrowVisible(true);
                    setTopDirectArrowVisible(true);
                    break;
                }
                case SHOW_BOTTOM: {
                    setBottomDirectArrowVisible(false);
                    setTopDirectArrowVisible(true);
                    break;
                }
            }
        }
        
        if (_impl) {
            _impl->updateShowState(showState);
        }
        
    }
}

void CommonTableView::tableCellTouched(TableView* table, TableViewCell* cell) {
    if (_isGridClicked) { //確保點擊的是Item，而非只是cell
        int lastSelectCellIndex = _selectGridIndex / _gridNum; //获取上一个选中的cell
        _selectGridIndex = (int)cell->getIdx() * _gridNum + _selectGridNum; //获取当前选中的grid index
        table->updateCellAtIndex(lastSelectCellIndex);
        table->updateCellAtIndex(cell->getIdx());
        _isGridClicked = false;
        
        if (_impl) {
            _impl->onGridItemClicked(_selectGridIndex);
        }
    }
}

Size CommonTableView::tableCellSizeForIndex(TableView *table, ssize_t idx) {
    if (_direction == VERTICAL) {
        int cellHeight = _itemHeight + _itemVerticalSpace;
        if (idx + 1 == numberOfCellsInTableView(table)) { //最後一行不應包含豎直間距（_itemVerticalSpace）
            cellHeight = _itemHeight;
        }
        return Size(_gridWidth, cellHeight);
    } else {
        int cellWidth = _itemWidth + _itemHorizontalSpace;
        if (idx + 1 == numberOfCellsInTableView(table)) { //最後一行不應包含橫向間距（_itemVerticalSpace）
            cellWidth = _itemWidth;
        }
        return Size(cellWidth, _gridHeight);
    }
}

TableViewCell* CommonTableView::tableCellAtIndex(TableView *table, ssize_t idx) {
    TableViewCell *cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();

        //布局
        Layout* cellLayout = Layout::create();
        cellLayout->setContentSize(Size(_gridWidth, _itemHeight));
        cellLayout->setBackGroundImageScale9Enabled(true);
        cellLayout->setName("cellLayout");
        cell->addChild(cellLayout);

        for (int i = 0; i < _gridNum; ++i) {
            Layout* gridLayout = Layout::create();
            gridLayout->setLayoutType(Layout::Type::ABSOLUTE);
            gridLayout->setBackGroundImageScale9Enabled(true);
            gridLayout->setBackGroundImageCapInsets(Rect(15, 10, 18, 7));
            gridLayout->setContentSize(Size(_itemWidth, _itemHeight));
            gridLayout->addClickEventListener(CC_CALLBACK_1(CommonTableView::onGridItemClicked, this));
            gridLayout->setTouchEnabled(true);
            gridLayout->setSwallowTouches(false);
            gridLayout->setTag(i);
            cellLayout->addChild(gridLayout);
            
            if (_impl) {
                _impl->layoutItem(gridLayout); //交給子類處理
            }
        }
    }
    
    long itemAboveCount = idx * _gridNum; //當前行之前的所有Item總數
    bool hideStuLeft = false; //是否隱藏本行或本列剩餘的Item
    
    //填充
    for (int i = 0; i < _gridNum; ++i) {
        Layout* cellLayout = (Layout*) cell->getChildByName("cellLayout");
        Layout* gridLayout = (Layout*) cellLayout->getChildByTag(i);
        
        long index = idx * _gridNum + i;

        if (_direction == VERTICAL) {
            if (idx + 1 == numberOfCellsInTableView(table)) { //最后一行不包含竖向间距（_itemVerticalSpace）
                gridLayout->setPosition(Vec2(i * (_itemWidth + _itemHorizontalSpace), 0));
            } else {
                gridLayout->setPosition(Vec2(i * (_itemWidth + _itemHorizontalSpace), _itemVerticalSpace));
            }
        } else {
            if (idx == 0) { //第一列不包含横向间距（_itemHorizontalSpace）
                gridLayout->setPosition(Vec2(0, i * (_itemHeight + _itemVerticalSpace)));
            } else {
                gridLayout->setPosition(Vec2(_itemHorizontalSpace, i * (_itemHeight + _itemVerticalSpace)));
            }
        }
        
        bool isSelected = false;
        
        //獲取是否選中
        if (-1 < _selectGridIndex && _selectGridIndex == itemAboveCount + i) {
            isSelected = true;
        } else {
            isSelected = false;
        }
        
        /**
         * 最後一行Item數量小於_gridColumn的情況下
         * 隱藏剩餘部分的Item
         */
        if (itemAboveCount + i + 1 > _counts) {
            hideStuLeft = true;
        }

        gridLayout->setVisible(!hideStuLeft);
        
        //設置Item數據
        if (!hideStuLeft) {
            if (_impl) {
                _impl->setItemData(gridLayout, index, (int)idx, isSelected);
            }
        }
    }
    
    return cell;
}

ssize_t CommonTableView::numberOfCellsInTableView(TableView *table) {
    ssize_t row = _counts / _gridNum;
    if (0 < _counts % _gridNum) {
        row++;
    }
    return row;
}

void CommonTableView::onGridItemClicked(Ref* pSender) {
    Node* node = static_cast<Node*> (pSender);
    _selectGridNum = node->getTag();
    _isGridClicked = true;
}

void CommonTableView::setBasicData(int itemW, int itemH, int spaceH, int spaceV, int num, int gridViewSize, C_Directtion directtion) {
    _direction = directtion;
    _itemWidth = itemW;
    _itemHeight = itemH;
    _itemHorizontalSpace = spaceH;
    _itemVerticalSpace = spaceV;
    _gridNum = num;

    if (_direction == VERTICAL) {
        _gridWidth = (_itemWidth + _itemHorizontalSpace) * _gridNum - _itemHorizontalSpace;
        _gridHeight = gridViewSize;
    } else {
        _gridWidth = gridViewSize;
        _gridHeight = (_itemHeight + _itemVerticalSpace) * _gridNum - _itemVerticalSpace;
    }
}

void CommonTableView::setItemCounts(int counts) {
    _counts = counts;

    //数据重置
    _selectGridNum = -1;
    _selectGridIndex = -1;
    _isGridClicked = false;
    
    if (_tableView) {
        _tableView->reloadData();
    }
}

void CommonTableView::setDirectArrowVisible(bool visible) {
    _showDirectArrow = visible;
    if (_topArrow == nullptr) {
        _topArrow = ImageView::create("arrow_orange_top.png");
        if (_topArrow) {
            _topArrow->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
            _topArrow->setPosition(Vec2(_gridWidth + 3, _gridHeight));
            _topArrow->setVisible(false);
            this->addChild(_topArrow);
        }
    }
    if (_bottomArrow == nullptr) {
        _bottomArrow = ImageView::create("arrow_orange_bottom.png");
        if (_bottomArrow) {
            _bottomArrow->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            _bottomArrow->setPosition(Vec2(_gridWidth + 3, 0));
            _bottomArrow->setVisible(false);
            this->addChild(_bottomArrow);
        }
    }
}

void CommonTableView::setDirectArrowTexture(const string &topArrowStr, const string &bottomArrowStr) {
    if (_topArrow) {
        _topArrow->loadTexture(topArrowStr);
    }
    if (_bottomArrow) {
        _bottomArrow->loadTexture(bottomArrowStr);
    }
}

void CommonTableView::setDirectArrowPosition(const Vec2 &topPos, const Vec2 &bottomPos) {
    if (_topArrow) {
        _topArrow->setPosition(topPos);
    }
    if (_bottomArrow) {
        _bottomArrow->setPosition(bottomPos);
    }
}

void CommonTableView::setTopDirectArrowVisible(bool visible) {
    if (_topArrow) {
        _topArrow->setVisible(visible);
    }
}

void CommonTableView::setBottomDirectArrowVisible(bool visible) {
    if (_bottomArrow) {
        _bottomArrow->setVisible(visible);
    }
}

void CommonTableView::updateCellAtIndex(int index) {
    if (_tableView) {
        _tableView->updateCellAtIndex(index);
    }
}

void CommonTableView::reloadData() {
    if (_tableView) {
        _tableView->reloadData();
    }
}

void CommonTableView::setSelecteGridIndex(int selectGridIndex) {
    if (_tableView) {
        this->_selectGridIndex = selectGridIndex;
        int row = selectGridIndex / _gridNum;
        updateCellAtIndex(row);
    }
}

Vec2 CommonTableView::getContentOffset() {
    if (_tableView) {
        return _tableView->getContentOffset();
    }
    return Vec2(-1, -1);
}

void CommonTableView::setContentOffset(const Vec2 &offset, bool animated) {
    if (_tableView) {
        _tableView->setContentOffset(offset, animated);
    }
}
