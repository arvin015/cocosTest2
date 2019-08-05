//
// Created by mac_arvin on 2019-08-02.
//

#ifndef PROJ_ANDROID_STUDIO_DRAGDROPLAYER_H
#define PROJ_ANDROID_STUDIO_DRAGDROPLAYER_H

#include "BaseLayer.h"
#include "ui/UIButton.h"

class DragNode;

class DragDropLayer : public BaseLayer {
    typedef BaseLayer base;

public:

    DragDropLayer();
    virtual ~DragDropLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(DragDropLayer);

private:

    void dragEndHandle(const cocos2d::Vec2 &worldVec);

private:
    DragNode* curDragNode;
    cocos2d::ui::Button* deleteBtn;
};


#endif //PROJ_ANDROID_STUDIO_DRAGDROPLAYER_H
