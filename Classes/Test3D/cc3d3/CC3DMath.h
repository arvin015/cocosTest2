#ifndef __CC3DMATHH__
#define __CC3DMATHH__

#include "CC2DMath.h"

//=== === === === === === Types === === === === === === 
typedef unsigned short ushort;

//=== === === === === === Consts === === === === === === 
const float TEXTUREFIXSCALE = 1;

// ==== ==== ==== ==== ==== ==== ==== ==== Quat & Euler Functions ==== ==== ==== ==== ==== ==== ==== ====
inline void Quat_rotatePoint(const cocos2d::Quaternion &quat, const cocos2d::Vec3 &vin, cocos2d::Vec3 &vout) {
	vout = quat * vin;
}

inline cocos2d::Quaternion QuatFromAxisRadian(const cocos2d::Vec3& axis, float angle) {
	float halfAngle = angle * 0.5f;
	float sinHalfAngle = sinf(halfAngle);

	cocos2d::Quaternion q(
		axis.x * sinHalfAngle,
		axis.y * sinHalfAngle,
		axis.z * sinHalfAngle,
		cosf(halfAngle));
	return q;
}

inline cocos2d::Quaternion Z2Quat(float z) {
	return QuatFromAxisRadian(cocos2d::Vec3(0, 0, 1), z);
}
inline cocos2d::Quaternion X2Quat(float x) {
	return QuatFromAxisRadian(cocos2d::Vec3(1, 0, 0), x);
}
inline cocos2d::Quaternion Y2Quat(float y) {
	return QuatFromAxisRadian(cocos2d::Vec3(0, 1, 0), y);
}

inline cocos2d::Quaternion Eul2Quat(const cocos2d::Vec3 &rot) {
	//return	Z2Quat(rot.z) * Y2Quat(rot.y) * X2Quat(rot.x);
	//return	Y2Quat(rot.y) * X2Quat(rot.x) * Z2Quat(rot.z);
	return	Z2Quat(rot.z) * X2Quat(rot.x) * Y2Quat(rot.y);
}

Quaternion lookatQuat(Vec3 eye, Vec3 lookat, Vec3 up);

// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== Vector Functions ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== 
inline Vec2 vec2RotateAround(const Vec2 & in, const Vec2 & pos, float rad) {
	float sn = sin(rad);
	float cs = cos(rad);
	float xx = pos.x + (in.x - pos.x)*cs - (in.y - pos.y)*sn;
	float yy = pos.y + (in.y - pos.y)*cs + (in.x - pos.x)*sn;
	return Vec2(xx, yy);
}

inline void vec2Set(cocos2d::Vec2 &v, float x, float y) {
	v = cocos2d::Vec2(x, y);
}

inline void vec2Copy(cocos2d::Vec2 &v, cocos2d::Vec2 &ret) {
	ret = v;
}

inline float vectorLensq(const cocos2d::Vec3 &v) {
	return v.lengthSquared();
}

inline void vectorScale(const cocos2d::Vec3 &v, float scale, cocos2d::Vec3 &vout) {
	vout = v * scale;
}

inline void vectorSet(cocos2d::Vec3 &v, float x, float y, float z) {
	v = cocos2d::Vec3(x, y, z);
}

inline void vectorCopy(const cocos2d::Vec3 &v1, cocos2d::Vec3& ret) {
	ret = v1;
}

inline void vectorAddTo(cocos2d::Vec3& ret, const cocos2d::Vec3 &v2) {
	ret = ret + v2;
}

inline void vectorSubtract(const cocos2d::Vec3 &v1, const cocos2d::Vec3 &v2, cocos2d::Vec3& ret) {
	ret = v1 - v2;
}

inline float vectorDot(const cocos2d::Vec3 &v1, const cocos2d::Vec3 &v2) {
	return Vec3::dot(v1, v2);
}
inline void vectorCrossProduct(const cocos2d::Vec3 &v1, const cocos2d::Vec3 &v2, cocos2d::Vec3& ret) {
	Vec3::cross(v1, v2, &ret);
}

inline void normalize(cocos2d::Vec3& ret) {
	ret.normalize();
}

inline void avgnormal(const cocos2d::Vec3 &v1, const cocos2d::Vec3 &v2, cocos2d::Vec3 &ret) {
	ret = v1 + v2;
	ret.normalize();
}

inline cocos2d::Vec3 tri2normal(const cocos2d::Vec3 &vbase, const cocos2d::Vec3 &vhead, const cocos2d::Vec3 &vbase2) {
	cocos2d::Vec3 vup = vhead - vbase;
	cocos2d::Vec3 vflat = vbase2 - vbase;
	cocos2d::Vec3 ret;
	Vec3::cross(vflat, vup, &ret);
	ret.normalize();
	return ret;
}

inline void mkcrossnormal(const cocos2d::Vec3 &vbase, const cocos2d::Vec3 &vhead, const cocos2d::Vec3 &vbase2, cocos2d::Vec3 &ret) {
	cocos2d::Vec3 vup, vhorizon;
	vectorSubtract(vhead, vbase, vup); // find updir
	vectorSubtract(vbase2, vbase, vhorizon); // find horizondir
	vectorCrossProduct(vhorizon, vup, ret); // find normal
	normalize(ret); // unit lenght

}

inline void mknormal(const cocos2d::Vec3 &vbase, const cocos2d::Vec3 &vhead, cocos2d::Vec3 &ret) {
	cocos2d::Vec3 vup, vhorizon;
	vectorSubtract(vhead, vbase, vup); // find updir
	vectorCrossProduct(vup, vbase, vhorizon); // find horizondir
	vectorCrossProduct(vhorizon, vup, ret); // find normal
	normalize(ret); // unit lenght
}

// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== Matrix Functions ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== 
inline void lookatMatrix(const Vec3 &vEye, const Vec3 &vLookat, const Vec3 &vUp, Mat4 &fModelView) {
	Vec3 vN, vU, vV;

	vectorSubtract(vLookat, vEye, vN);

	vectorCrossProduct(vUp, vN, vU);

	normalize(vU);
	normalize(vN);

	vectorCrossProduct(vN, vU, vV);

	// create a model view matrix
	fModelView.m[0] = vU.x;
	fModelView.m[4] = vV.x;
	fModelView.m[8] = vN.x;
	fModelView.m[12] = vEye.x;
	fModelView.m[1] = vU.y;
	fModelView.m[5] = vV.y;
	fModelView.m[9] = vN.y;
	fModelView.m[13] = vEye.y;
	fModelView.m[2] = vU.z;
	fModelView.m[6] = vV.z;
	fModelView.m[10] = vN.z;
	fModelView.m[14] = vEye.z;
	fModelView.m[3] = 0.0f;
	fModelView.m[7] = 0.0f;
	fModelView.m[11] = 0.0f;
	fModelView.m[15] = 1.0f;
}

inline Vec3 unprojectNormalized(const Mat4 &unprojmatrix, const Vec3& src) {
	Vec4 nv4(src.x, src.y, src.z, 1.0f), v4out;
	unprojmatrix.transformVector(nv4, &v4out);
	if (v4out.w != 0.0f) {
		v4out.x /= v4out.w;
		v4out.y /= v4out.w;
		v4out.z /= v4out.w;
	}
	return Vec3(v4out.x, v4out.y, v4out.z);
}

#endif 
