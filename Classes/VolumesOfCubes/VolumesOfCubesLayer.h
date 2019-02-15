//
//  VolumesOfCubesLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/1/15.
//

#ifndef VolumesOfCubesLayer_h
#define VolumesOfCubesLayer_h

#include <stdio.h>
#include "BaseLayer.h"
#include "cMultiTouch.h"
#include "DrawNode3D.h"

class CC3DLayer;
class Face;

class VolumesOfCubesLayer : public BaseLayer {

public:

    VolumesOfCubesLayer();
    virtual ~VolumesOfCubesLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(VolumesOfCubesLayer);

    virtual bool shouldWhiteBg() {return false;};

    void createCube(const cocos2d::Vec3 &position, const cocos2d::Color3B &fillColor);
    
private:
    
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

private:

    cocos2d::EventListenerTouchOneByOne* touchListener;
    cocos2d::DirectionLight* light;
    CC3DLayer* cc3DLayer;
    cocos2d::Vec3 camtarget, camoffset;
    cocos2d::Quaternion camquat;
    cMultiTouch multitouch;
};

#endif /* VolumesOfCubesLayer_h */
