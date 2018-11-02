#ifndef HITTESTH
#define HITTESTH

//http://geomalgorithms.com/a07-_distance.html

#include "cocos2d.h"


float hittestlinepoint(const cocos2d::Vec3 &vorig, const cocos2d::Vec3 &vdir,
                       const cocos2d::Vec3 &vp);

void hittestlinetri(const cocos2d::Vec3 &orig, const cocos2d::Vec3 &dir,
                    const cocos2d::Vec3 &vert0, const cocos2d::Vec3 &vert1, const cocos2d::Vec3 &vert2,
                    bool *o_hit, float *o_dist);

float hittestlineline2(const cocos2d::Vec3 &p1, const cocos2d::Vec3 &q1,
                       const cocos2d::Vec3 &p2, const cocos2d::Vec3 &q2,
	cocos2d::Vec3 &x1, cocos2d::Vec3 &x2);

float hittestlineline(const cocos2d::Vec3 &v1a, const cocos2d::Vec3 &v1b,
                      const cocos2d::Vec3 &v2a, const cocos2d::Vec3 &v2b, cocos2d::Vec3 &v);

    
#endif