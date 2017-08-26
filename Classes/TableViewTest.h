//
//  TableViewTest.h
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#ifndef TableViewTest_h
#define TableViewTest_h

#include <stdio.h>
#include "Main/BaseLayer.h"
#include "DatasetInfo.h"
#include "ui/UIRadioButton.h"

class DatasetTableView;

class TableViewTest : public BaseLayer {

public:
    TableViewTest();
    ~TableViewTest();
    
    virtual bool init();
    
    CREATE_FUNC(TableViewTest);

private:
    void onRadioButtonSelect(cocos2d::ui::RadioButton* radioButton, int index, cocos2d::ui::RadioButtonGroup::EventType eventType);
    
private:
    DatasetTableView* tableView;
    std::vector<DatasetInfo> datasetList;
};

#endif /* TableViewTest_h */
