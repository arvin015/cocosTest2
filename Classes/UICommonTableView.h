/*
 * UICommonTableView.h
 *
 */

#ifndef _UI_COMMON_TABLE_VIEW_H_
#define _UI_COMMON_TABLE_VIEW_H_

#include <string>
#include <vector>
#include <map>

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/UILayout.h"
#include "TableViewImpl.h"

class CommonTableView :
		public cocos2d::Node,
		public cocos2d::extension::TableViewDataSource,
		public cocos2d::extension::TableViewDelegate
{
    
public:
    
 	CommonTableView();
	~CommonTableView();

	CREATE_FUNC(CommonTableView);

	virtual bool init();
    virtual void cleanup();

	virtual void onEnter();

	virtual void onExit();
    
	/* -------------------- 必須實現的方法 -------------------- */

	// 當滑動TableView時觸發該方法
	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view){};

	// 當TableView被放大或縮小時觸發該方法
	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view){};

	// 當cell被點擊时觸發該方法
	virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);

	// 設置TableView cell的大小
	virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx);

	// 獲取編號為idx的cell
	virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx);

	// 設置TableView中cell的個數
	virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table);

	/* -------------------- 可選實現的方法 -------------------- */

    // 設置基礎數據
    void setBasicData(int itemW, int itemH, int spaceH, int spaceV, int column);
    
    // 設置Item總數
    void setItemCounts(int counts);
    
    // Item點擊
    virtual void onSelectIndex(int selectIndex) {};
    
    // 獲取當前選中的Index
    int getSelectedIndex() {
        return _selectGridIndex;
    }

	// 取得TableView
    cocos2d::extension::TableView* getTableView() {
        return _tableView;
    }

    // 設置操作接口
    void setTableViewImpl(TableViewImpl* impl) {
        this->_impl = impl;
    }
    
private:
    
    // 點擊Item處理
    void onGridItemClicked(cocos2d::Ref* pSender);
    
private:
    
	int GRID_ITEM_WIDTH = 280;// 每個Grid的寬度
	int GRID_ITEM_HEIGHT = 250;// 每個Grid的高度
	int GRID_ITEM_SPACE_HORIZONTAL = 10;// 每個Grid橫向之間的間距
	int GRID_ITEM_SPACE_VERTICAL = 10;// 每個Grid豎向之間的間距
    int COUNTS; // Item總個數

	int GRID_VIEW_COLUMN = 3;// 整個GridView的包含的列數
	int GRID_VIEW_WIDTH = (GRID_ITEM_WIDTH + GRID_ITEM_SPACE_HORIZONTAL) * GRID_VIEW_COLUMN - GRID_ITEM_SPACE_HORIZONTAL;// GridView的寬度
	int GRID_VIEW_HEIGHT = 560;
    
	int _selectGridColumn;// 選中的GridView的列，由0開始編號
	int _selectGridIndex;// 選中的格子對應的位置，由0開始編號

	bool _isGridClicked;// Grid Item是否被點擊

    TableViewImpl* _impl; // 操作接口
	cocos2d::extension::TableView* _tableView;// 實現GridView的TableView
};

#endif /* _UI_COMMON_TABLE_VIEW_H_ */
