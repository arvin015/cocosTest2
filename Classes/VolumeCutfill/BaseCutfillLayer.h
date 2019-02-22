//
//  BaseCutfillLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/21.
//

#ifndef BaseCutfillLayer_h
#define BaseCutfillLayer_h

#include <stdio.h>
#include "cocos2d.h"
#include "VolumeQuestion.h"
#include "Solid3D.h"
#include "CC3DLayer.h"
#include "UIDrawNodeEx.h"
#include "MathUtils.h"
#include "json/document.h"

class BaseCutfillLayer : public cocos2d::Layer {

public:

    const float MaxDis = 50;

    BaseCutfillLayer();
    virtual ~BaseCutfillLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    void setData(CC3DLayer* cc3DLayer, VolumeQuestion* question);

    void setTouchEnabled(bool touchEnabled) {
        this->touchEnabled = touchEnabled;
    }

    virtual void toJson(rapidjson::Document &json);
    virtual void fromJson(const rapidjson::Value &json);

    virtual void reset();

    virtual void clear();

protected:

    Solid3D* createSolid3D(const std::vector<VertexInfo> &vecs);

    VertexInfo getStartPoint(const cocos2d::Vec2 &point);

protected:

    CC3DLayer* cc3DLayer;
    cocos2d::Node* _3dContainer;

    bool touchEnabled;

    DrawNodeEx* drawNodeEx;

    Solid3D* curSolid3D;
    VertexInfo startVertex;

    VolumeQuestion* question;
    cocos2d::Vector<Solid3D*> solid3DList;
};

#endif /* BaseCutfillLayer_h */
