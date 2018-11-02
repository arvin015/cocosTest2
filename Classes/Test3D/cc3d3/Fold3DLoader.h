#ifndef __Fold3DLoaderH__
#define __Fold3DLoaderH__

//#include "CC2D3Util.h"
#include "CC3DMath.h"

class Sprite3DAni;

std::vector<cocos2d::Sprite3D*> ObjList_Load(cocos2d::Node *rootnode, const char *json, const std::string &basepath);
void ObjList_AniLoad(Sprite3DAni &ani, const char *json);
void ObjList_AniApply(const std::vector<cocos2d::Sprite3D*> &objs, const Sprite3DAni &ani, float time);

cocos2d::Sprite3D* addSprite3D(cocos2d::Node *pnode, const char *objname, const char *pngname, const float scale, const cocos2d::Vec3 &pos, const cocos2d::Vec3 &rot);

typedef struct {
	float time;
	cocos2d::Vec3 euler;
}Sprite3DAniKeyRot;
typedef std::map<float, cocos2d::Vec3> Sprite3DTrackRot;
typedef std::map<int, Sprite3DTrackRot> Sprite3DTrackRotMap;

typedef struct {
	float time;
	cocos2d::Vec3 pos;
}Sprite3DAniKeyPos;
typedef std::map<float, cocos2d::Vec3> Sprite3DTrackPos;
typedef std::map<int, Sprite3DTrackPos> Sprite3DTrackPosMap;

typedef struct {
	float time;
	float scale;
}Sprite3DAniKeyScale;
typedef std::map<float, float> Sprite3DTrackScale;
typedef std::map<int, Sprite3DTrackScale> Sprite3DTrackScaleMap;

class Sprite3DAni {
public:
	float maxtime, curtime;
	Sprite3DTrackScaleMap scaletracks;
	Sprite3DTrackRotMap rottracks;
	Sprite3DTrackPosMap postracks;
public:
	void free() {
		curtime = 0;
		maxtime = 0;
		scaletracks.clear();
		rottracks.clear();
		postracks.clear();
	}
};

inline void Sprite3DAniScale(cocos2d::Sprite3D *sp3d, const Sprite3DTrackScale &t, float v) {
	Sprite3DTrackScale::const_iterator itr = t.lower_bound(v);
	if (itr == t.begin()) {
		sp3d->setScale(itr->second);
	}
	else if (itr == t.end()) {
		itr--;
		sp3d->setScale(itr->second);
	}
	else {
		//float t2 = itr->first;
		float s2 = itr->second;
		itr--;
		float t1 = itr->first;
		float s1 = itr->second;
		sp3d->setScale(s1 + (v - t1) * (s2 - s1));
	}
}

inline void Sprite3DAniPos(cocos2d::Sprite3D *sp3d, const Sprite3DTrackPos &t, float v) {
	Sprite3DTrackPos::const_iterator itr = t.lower_bound(v);
	if (itr == t.begin()) {
		sp3d->setPosition3D(itr->second);
	}
	else if (itr == t.end()) {
		itr--;
		sp3d->setPosition3D(itr->second);
	}
	else {
		//float t2 = itr->first;
		const cocos2d::Vec3 &p2 = itr->second;
		itr--;
		float t1 = itr->first;
		const cocos2d::Vec3 &p1 = itr->second;
		sp3d->setPosition3D(p1 + (v - t1) * (p2 - p1));
	}
}

inline void Sprite3DAniRot(cocos2d::Sprite3D *sp3d, const Sprite3DTrackRot &t, float v) {
	Sprite3DTrackRot::const_iterator itr = t.lower_bound(v);
	if (itr == t.begin()) {
		sp3d->setRotationQuat(Eul2Quat(itr->second));
	}
	else if (itr == t.end()) {
		itr--;
		sp3d->setRotationQuat(Eul2Quat(itr->second));
	}
	else {
		//float t2 = itr->first;
		cocos2d::Quaternion q2 = Eul2Quat(itr->second);
		itr--;
		float t1 = itr->first;
		cocos2d::Quaternion q1 = Eul2Quat(itr->second);
		cocos2d::Quaternion qtween;
		cocos2d::Quaternion::slerp(q1, q2, (v - t1), &qtween);
		sp3d->setRotationQuat(qtween);
	}
}


inline void maptest() {
	Sprite3DTrackScale t;
/*
	t[0] = 0;
	maptesttween(t, -1);
	maptesttween(t, 0);
	maptesttween(t, 1);
	t[2] = 2;
	maptesttween(t, -1);
	maptesttween(t, 0);
	maptesttween(t, 1);
	maptesttween(t, 2);
	maptesttween(t, 3);
*/
}


#endif 
