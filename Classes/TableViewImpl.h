//
//  TableViewImpl.h
//  CocosTest2
//
//  Created by arvin on 2017/8/18.
//
//

#ifndef TableViewImpl_h
#define TableViewImpl_h

class TableViewImpl {
    
public:
    
    /**
     * 设置Item布局
     */
    virtual void layoutItem(cocos2d::ui::Layout* itemLayout) = 0;
    
    /**
     * 设置Item数据
     */
    virtual void setItemData(cocos2d::ui::Layout* itemLayout, long itemIndex, bool isSelected) = 0;
};


#endif /* TableViewImpl_h */
