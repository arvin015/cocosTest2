#ifndef __CC3DLayerH__
#define __CC3DLayerH__

#include "cocos2d.h"

class cMultiTouch;

USING_NS_CC;

class CC3DLayer: public cocos2d::Layer {
	typedef Layer base;
	typedef CC3DLayer thisclass;

public:
	cocos2d::Camera *_camera;
	cocos2d::Vec3 camtarget, camoffset;
	cocos2d::Quaternion camquat;
protected:
	CameraFlag camFlag;
	int camMask;

public:
	virtual bool init();

	void userCameraFlag(CameraFlag flag);
	void setCamLoc(const cocos2d::Vec3 &camtarget_, const cocos2d::Quaternion &camquat_, const cocos2d::Vec3 &camoffset_);
	void updateCamLoc();

	void drag3d(Node *touchnode, Touch *t, cMultiTouch &multitouch);

	CREATE_FUNC(CC3DLayer);
protected:
	CC3DLayer() :base(),
		_camera(nullptr),
		camFlag(CameraFlag::USER1),	
		camMask((int)CameraFlag::USER1)
	{};
	virtual ~CC3DLayer();

	bool onTouchBegan(cocos2d::Touch* t, cocos2d::Event* e);
	void onTouchMoved(cocos2d::Touch* t, cocos2d::Event* e);
	void onTouchEnded(cocos2d::Touch* t, cocos2d::Event* e);
	void onTouchCancelled(cocos2d::Touch* t, cocos2d::Event* e);

};


#endif 
