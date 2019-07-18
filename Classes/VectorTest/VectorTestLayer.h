//
// Created by Administrator on 2019/6/1.
//

#ifndef PROJ_ANDROID_STUDIO_VECTORTESTLAYER_H
#define PROJ_ANDROID_STUDIO_VECTORTESTLAYER_H

#include "BaseLayer.h"

class PolyView;

class VectorTestLayer : public BaseLayer {

    typedef BaseLayer base;

public:
    VectorTestLayer();
    ~VectorTestLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();
    CREATE_FUNC(VectorTestLayer);

private:

    PolyView* createPolyView(const cocos2d::Vec2 &centerP, float radius, int polyType);

private:
    cocos2d::Vector<PolyView*> polyViewList;
    PolyView* curPolyView;
};


#endif //PROJ_ANDROID_STUDIO_VECTORTESTLAYER_H
