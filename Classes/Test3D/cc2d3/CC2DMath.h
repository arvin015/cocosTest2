#ifndef CC2D3MATHUTILH__
#define CC2D3MATHUTILH__

#include "cocos2d.h"
USING_NS_CC;

//2D Line Functions
bool lineCrossed(const Vec2 &line1a, const Vec2 &line1b, const Vec2 &line2a, const Vec2 &line2b);

//2D Polygon Shape Functions
float shapeAreaSigned2(const std::vector<Vec2> &pts); //for Close Shape only (last Vec overlap first)

inline float shapeAreaSigned(const std::vector<Vec2> pts) {
	return shapeAreaSigned2(pts) * 0.5f;
}
inline float shapeArea(const std::vector<Vec2>& pts) {
	return fabs(shapeAreaSigned2(pts) * 0.5f);
}

inline bool shapeIsClockwise(const std::vector<Vec2> &pts) {
	return 0 >= shapeAreaSigned2(pts); //arvin edit add'='
}


// Angle Functions ===== ===== ===== ===== ===== ===== ===== 
inline float Deg2Rad(float d) {
	return CC_DEGREES_TO_RADIANS(d);
}

inline float Rad2Deg(float r) {
	return CC_RADIANS_TO_DEGREES(r);
}

inline float RadWrap(float r) {
	while (r >= M_PI)
		r -= (float)(M_PI * 2.0f);
	while (r < -M_PI)
		r += (float)(M_PI * 2.0);
	return r;
}

#endif
