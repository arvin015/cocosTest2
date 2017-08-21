//
//  UIDatasetTableView.h
//  CocosTest2
//
//  Created by arvin on 2017/8/18.
//
//

#ifndef UIDatasetTableView_h
#define UIDatasetTableView_h

#include <stdio.h>
#include "UICommonTableView.h"

class OnGridItemClickedListener
{
public:
    
    virtual void onGridItemClicked(int index) = 0; //点击Item回调
    
    virtual ~OnGridItemClickedListener(){}
};

class DatasetInfo;

class DatasetTableView : public CommonTableView, TableViewImpl {
    
public:
    
    DatasetTableView();
    ~DatasetTableView();
    
    virtual void onEnter();
    virtual void onExit();

    CREATE_FUNC(DatasetTableView);
    virtual bool init();
    
    /**
     * 设置Item布局
     */
    virtual void layoutItem(cocos2d::ui::Layout* itemLayout);
    
    /**
     * 设置Item数据
     */
    virtual void setItemData(cocos2d::ui::Layout* itemLayout, long itemIndex, bool isSelected);
    
    /**
     * Item点击处理
     */
    virtual void onSelectIndex(int selectIndex);
    
    /**
     * 设置数据
     */
    void setData(std::vector<DatasetInfo> datasetList);
    
    /**
     * 设置选中回调
     */
    void setSelectListener(OnGridItemClickedListener* listener) {
        this->listener = listener;
    }
    
private:
    
    OnGridItemClickedListener* listener; //点击单元格回调函数
    std::vector<DatasetInfo> datasetList;
};

#endif /* UIDatasetTableView_h */