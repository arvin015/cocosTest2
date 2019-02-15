#include "CC3DLayer.h"

//#include "CC2D3Util.h"
#include "CC3DMath.h"
#include "cMultiTouch.h"
#include "LogUtil.h"

USING_NS_CC;

template <typename LightType>
LightType *addLight(Node *pnode, bool enable, int cammask, LightType *light) {
	pnode->addChild(light);
	light->setEnabled(enable);
	light->setCameraMask(cammask);
	return light;
}

void addLights(Node *pnode, int cammask) {
	addLight(pnode, true, cammask, AmbientLight::create(Color3B(100, 120, 120)));
	addLight(pnode, true, cammask, DirectionLight::create(Vec3(1.0f, 0.5f, -0.8f), Color3B(220, 200, 200)));
	addLight(pnode, true, cammask, PointLight::create(Vec3(30.0f, 15.0f, -20.0f), Color3B(160, 140, 120), 1000.0f));
	/*
	{
	auto tintto1 = TintTo::create(4, 0, 0, 255);
	auto tintto4 = TintTo::create(4, 255, 255, 255);
	auto seq = Sequence::create(tintto1, tintto4, nullptr);
	_ambientLight->runAction(RepeatForever::create(seq));
	}{
	auto _directionalLight = addLight(pnode, false, Kcammask, DirectionLight::create(Vec3(-1.0f, -1.0f, 0.0f), Color3B(200, 200, 200)));
	auto tintto1 = TintTo::create(4, 255, 0, 0);
	auto tintto4 = TintTo::create(4, 255, 255, 255);
	auto seq = Sequence::create(tintto1, tintto4, nullptr);
	_directionalLight->runAction(RepeatForever::create(seq));
	}{
	auto tintto1 = TintTo::create(4, 255, 0, 0);
	auto tintto4 = TintTo::create(4, 255, 255, 255);
	auto seq = Sequence::create(tintto1, tintto4, nullptr);
	_pointLight->runAction(RepeatForever::create(seq));
	}{
	auto _spotLight = addLight(pnode, false, Kcammask, SpotLight::create(Vec3(-1.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Color3B(200, 200, 200), 0.0, 0.5, 10000.0f));
	auto tintto1 = TintTo::create(4, 255, 0, 0);
	auto tintto4 = TintTo::create(4, 255, 255, 255);
	auto seq = Sequence::create(tintto1, tintto4, nullptr);
	_spotLight->runAction(RepeatForever::create(seq));
	}
	*/
}

void CC3DLayer::userCameraFlag(CameraFlag flag) {
	camFlag = flag;
	camMask = (unsigned short)flag;
	_camera->setCameraFlag(camFlag);
	setCameraMask(camMask);
}


bool CC3DLayer::init() {
	if (!base::init()) {
		return false;
	}

	auto s = Director::getInstance()->getWinSize();
		
	//addLights(this, camMask);

	//addDrawGrid(this);
	
	_camera = Camera::createPerspective(60, (GLfloat)s.width / s.height, 1, 1001);

	_camera->setCameraFlag(camFlag);
	setCameraMask(camMask);

	setCamLoc(Vec3(0, 0, 0), X2Quat(0), Vec3(0, 0, 5));
	
	addChild(_camera);
	/*
	Quaternion _rotationQuat.set(0,0,0,1);

	//calculateArcBall(axes, angle, prelocation.x, prelocation.y, location.x, location.y);    //calculate  rotation quaternion parameters
	Quaternion quat(Vec3(0.f, 0.f, 1.f), 1.5f - pi * 0.5f, &quat);
	_rotationQuat = quat * _rotationQuat;
	_sprite->setRotationQuat(quat);
	*/
	return true;
}

void CC3DLayer::setCamLoc(const Vec3 &camtarget_, const Quaternion &camquat_, const Vec3 &camoffset_){
	camtarget = camtarget_;
	camquat = camquat_;
	camoffset = camoffset_;
	updateCamLoc();
}

void CC3DLayer::updateCamLoc() {
	_camera->setRotationQuat(camquat);
	_camera->setPosition3D(camtarget + (camquat * camoffset));
}

void logunp(const Mat4 &unprojmatrix, const char *pref, const Vec3 &v) {
	Vec3 unp = unprojectNormalized(unprojmatrix, v);
	LOGUTIL(LEVEL_INFO, tagtag::TAG(),"%s (%f, %f, %f)", pref, unp.x, unp.y, unp.z);
}
bool CC3DLayer::onTouchBegan(Touch* t, Event* e) {
	Vec2 ttn = convertTouchToNodeSpaceAR(t);
	//y + => top, - => bottom
	//x - => left, + => right

	Mat4 m4unproj = _camera->getViewProjectionMatrix().getInversed();

	logunp(m4unproj, "near", Vec3(0, 0, -1));
	logunp(m4unproj, "mid ", Vec3(0, 0, 0));
	logunp(m4unproj, "far ", Vec3(0, 0, 1));
	logunp(m4unproj, "topN", Vec3(0, -1, -1));
	logunp(m4unproj, "topF", Vec3(0, -1, 1));
	logunp(m4unproj, "top ", Vec3(0, -1, 0));
	logunp(m4unproj, "LFT ", Vec3(-1, 0, 0));
	logunp(m4unproj, "RGT ", Vec3(1, 0, 0));

	//Vec3 unp = _camera->unproject(Vec3(0, 0, -1));
	//Vec3 unp2 = _camera->unproject(Vec3(0, 0, 0));
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"Touch Begin pV= (%f, %f, %f) pv2= (%f, %f, %f)", unp.x, unp.y, unp.z, unp2.x, unp2.y, unp2.z);

	//Vec3 unpg = _camera->unprojectGL(Vec3(0, 0, -1));
	//Vec3 unpg2 = _camera->unprojectGL(Vec3(0, 0, 0));
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"Touch Begin pG= (%f, %f, %f) pG=2 (%f, %f, %f)", unpg.x, unpg.y, unpg.z, unpg2.x, unpg2.y, unpg2.z);

	Vec4 pro;
	_camera->getViewProjectionMatrix().transformVector(Vec4(0, 1, 0, 1.0f), &pro);
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"worldmove %f, %f, uup = %f,%f,%f, proj = %f,%f,%f", 
	//	delta.x, delta.y, unp.x, unp.y, unp.z, pro.x, pro.y, pro.z);

	// z -1 ~ 1 = near ~ far
	return true;
}
void CC3DLayer::onTouchMoved(Touch* t, Event* e) {
	Vec2 delta = t->getDelta();
//	_camera->setPosition3D(Vec3(0, -40, 25));
//	_camera->lookAt(Vec3(0, 0, 0));
	
	Quaternion qx = X2Quat(delta.y * 0.02f);
	Quaternion qy = Y2Quat(delta.x * -0.02f);
	camquat = camquat * qx * qy;
	updateCamLoc();
}
void CC3DLayer::onTouchEnded(Touch* t, Event* e) {}
void CC3DLayer::onTouchCancelled(Touch* t, Event* e) {}

CC3DLayer::~CC3DLayer() {}

inline Director* DirectorInstance()        {    return Director::getInstance(); }

inline cocos2d::Size ScreenSize()    {     return DirectorInstance()->getVisibleSize(); }

inline Vec2 GLNormalized(const Vec2 &v) {
	cocos2d::Size size = ScreenSize();
	return Vec2((2.0f * v.x / size.width), (2.0f * v.y / size.height));
}

void CC3DLayer::drag3d(Node *touchnode, Touch *t, cMultiTouch &multitouch) {
	if (multitouch.active) {
		if (multitouch.wasactive) {
			Quaternion qz = Z2Quat(multitouch.mtLast.rad - multitouch.mt.rad);
			camquat = camquat * qz;
		}
	} else {
		Vec2 t1 = GLNormalized(touchnode->convertToNodeSpaceAR(t->getLocation()));

		Vec2 delta = GLNormalized(t->getDelta());
		Quaternion qx = X2Quat(delta.y * 2.0f * (1 - (t1.x * t1.x)));
		Quaternion qy = Y2Quat(-delta.x * 2.0f * (1 - (t1.y * t1.y)));

		Vec2 t2 = GLNormalized(touchnode->convertToNodeSpaceAR(t->getPreviousLocation()));
		float r1 = atan2f(t1.y, t1.x);
		float r2 = atan2f(t2.y, t2.x);
		//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"Touch move a1=%f a2=%f", atan2f(t1.y, t1.x), atan2f(t2.y, t2.x));
		camquat = camquat * (qx * qy) * Z2Quat(RadWrap(r2 - r1) * t1.lengthSquared());
	}
	updateCamLoc();
}

