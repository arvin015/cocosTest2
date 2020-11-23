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
#include "ui/UIImageView.h"

enum ShowState {
	SHOW_ALL, //完全显示
	SHOW_TOP, //显示头部，尾部未显示
	SHOW_BOTTOM, //显示尾部，头部未显示
	SHOW_CENTER, //显示中间，头尾都未显示
};

class TableViewImpl {
    
public:
    
    /**
     * 设置Item布局
     */
    virtual void layoutItem(cocos2d::ui::Layout* itemLayout) = 0;
    
    /**
     * 设置Item数据
     */
    virtual void setItemData(cocos2d::ui::Layout* itemLayout, long itemIndex, int row, bool isSelected) = 0;
    
    /**
     * 点击Item回调
     */
    virtual void onGridItemClicked(int index) {};
    
    /**
     * 显示状态改变
     */
    virtual void updateShowState(ShowState showState) {};
};

class CommonTableView : public cocos2d::Node,
public cocos2d::extension::TableViewDataSource,
public cocos2d::extension::TableViewDelegate
{
    
public:

	enum C_Directtion {
		HORIZONTAL,
		VERTICAL
	};

 	CommonTableView();
	~CommonTableView();

	CREATE_FUNC(CommonTableView);

	virtual bool init();
    virtual void cleanup();

	virtual void onEnter();

	virtual void onExit();

	/**
	 * 當滑動TableView時觸發該方法
	 */
	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view);

	/**
	 * 當TableView被放大或縮小時觸發該方法
	 */
	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view){};

	/**
	 * 當cell被點擊时觸發該方法
	 */
	virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);

	/**
	 * 設置TableView cell的大小
	 */
	virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx);

	/**
	 * 獲取編號為idx的cell
	 */
	virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx);

	/**
	 * 設置TableView中cell的個數
	 */
	virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table);

	/* -------------------- 可選實現的方法 -------------------- */

	/**
	 * 获取方向
	 */
	C_Directtion getDirection() {
		return _direction;
	}

    /**
     * 設置基礎數據
     * @itemW        单位宽
     * @itemH        单位高
     * @spaceH       横间距
     * @spaceV       竖间距
     * @num          方向为VERTICAL时表示列数，为HORIZONTAL时表示行数
     * @gridViewSize 方向为VERTICAL时表示总高，为HORIZONTAL时表示总宽
     * @directtion   布局方向，默认为VERTICAL
     */
    void setBasicData(int itemW, int itemH, int spaceH, int spaceV, int num, int gridViewSize, C_Directtion directtion = VERTICAL);
    
    /**
     * 設置Item總數
     */
    void setItemCounts(int counts);
    
    /**
     * 獲取當前選中的Index
     */
    int getSelectedIndex() {
        return _selectGridIndex;
    }

	/**
	 * 取得TableView
	 */
    cocos2d::extension::TableView* getTableView() {
        return _tableView;
    }

    /**
     * 設置操作接口
     */
    void setTableViewImpl(TableViewImpl* impl) {
        this->_impl = impl;
    }
    
    /**
     * 設置是否需要顯示指示箭頭
     */
    void setDirectArrowVisible(bool visible);
    
    /**
     * 更改指示箭頭
     */
    void setDirectArrowTexture(const std::string &topArrowStr, const std::string &bottomArrowStr);
    
    /**
     * 更改指示箭頭位置
     */
    void setDirectArrowPosition(const cocos2d::Vec2 &topPos, const cocos2d::Vec2 &bottomPos);
    
    /**
     * 設置上箭頭顯示隱藏
     */
    void setTopDirectArrowVisible(bool visible);
    
    /**
     * 設置下箭頭顯示隱藏
     */
    void setBottomDirectArrowVisible(bool visible);
    
    /**
     * 更新指定行
     */
    void updateCellAtIndex(int index);
    
    /**
     * 更新整個TableView
     */
    void reloadData();
    
    /**
     * 获取总列数或总行数
     */
    int getTotalColumn() {
        return _gridNum;
    }

	/**
	 * 設置指定單元格被選中
	 */
	void setSelecteGridIndex(int selectGridIndex);

    /**
     * 获取滑动位置
     */
    cocos2d::Vec2 getContentOffset();

    /**
     * 设置滑动位置
     */
    void setContentOffset(const cocos2d::Vec2 &offset, bool animated = false);

    /**
     * 父Node大小是否需要通过子TableView来设定，如果是则Node设置的setContentSize无效
     */
    void setAdapterByGrid(bool isAdapterByGrid) {
        this->isAdapterByGrid = isAdapterByGrid;
    }
    
private:
    
    /**
     * 點擊Item處理
     */
    void onGridItemClicked(cocos2d::Ref* pSender);
    
private:
	C_Directtion _direction = VERTICAL; //方向
	int _itemWidth; //Item的寬度
	int _itemHeight; //Item的高度
	int _itemHorizontalSpace; //Item之間的橫向間距
	int _itemVerticalSpace; //Item之間的縱向間距
    int _counts = 0; //Item總個數

    bool isAdapterByGrid = false; //父Node大小是否需要通过子TableView来设定

	int _gridNum = 0; //方向为VERTICAL表示GridView包含的列数，方向为HORIZONTAL表示行数
	int _gridWidth; //GridView的寬度
	int _gridHeight; //GridView的高度
    
	int _selectGridNum; //方向为VERTICAL表示选中的GridView列，方向为HORIZONTAL表示选中的GridView行，由0開始編號
	int _selectGridIndex; //選中的格子對應的位置，由0開始編號

	bool _isGridClicked; //Grid Item是否被點擊
    bool _showDirectArrow; //是否顯示指示箭頭

    TableViewImpl* _impl; //操作接口
	cocos2d::extension::TableView* _tableView; //實現GridView的TableView
    cocos2d::ui::ImageView* _topArrow; //上箭頭
    cocos2d::ui::ImageView* _bottomArrow; //下箭頭
};

#endif /* _UI_COMMON_TABLE_VIEW_H_ */
