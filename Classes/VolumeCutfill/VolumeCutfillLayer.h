//
//  VolumeCutfillLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/12.
//

#ifndef VolumeCutfillLayer_h
#define VolumeCutfillLayer_h

#include <stdio.h>
#include "BaseLayer.h"
#include "eGeom3D.h"
#include "cMultiTouch.h"

class CC3DLayer;
class Volume3D;

class VolumeCutfillLayer : public BaseLayer {
  
public:
    
    VolumeCutfillLayer();
    virtual ~VolumeCutfillLayer();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(VolumeCutfillLayer);
    
    virtual bool shouldWhiteBg() {return false;};
    
private:
    
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    void createVolume3D(const std::vector<cocos2d::Vec2> &vecs, const cocos2d::Vec3 &offset = Vec3::ZERO, bool playAnim = false);

    cocos2d::Vec2 getStartPoint(const cocos2d::Vec2 &point);

    cocos2d::Vec2 getEndPoint(const cocos2d::Vec2 &point, const cocos2d::Vec2 &startPoint);

private:
    
    cocos2d::EventListenerTouchOneByOne* touchListener;
    CC3DLayer* cc3DLayer;
    cocos2d::DirectionLight* light;
    cocos2d::Vec3 camtarget, camoffset;
    cocos2d::Quaternion camquat;
    cMultiTouch multitouch;
    bool canRotateCamera;

    cocos2d::DrawNode* drawNode;

    Volume3D* curVolume3D;
    cocos2d::Vector<Volume3D*> volume3DList;
};

#endif /* VolumeCutfillLayer_h */
