//
//  Cube3D.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/1/15.
//

#ifndef Cube3D_h
#define Cube3D_h

#include <stdio.h>
#include "cocos2d.h"

class Vertex {
public:
    Vertex() {};
    Vertex(const cocos2d::Vec3& pos, const cocos2d::Vec2& uv) : Vertex() {
        this->position = pos;
        this->uv = uv;
    }
    ~Vertex() {};

    cocos2d::Vec3 position;
    cocos2d::Vec2 uv;
};

class Face {

public:
    Face(const cocos2d::Vec3 &p1, const cocos2d::Vec3 &p2, const cocos2d::Vec3 &p3, const cocos2d::Vec3 &p4, const cocos2d::Color4F &color);
    virtual ~Face();

    cocos2d::Vec3 getNormal();
    cocos2d::Vec3 getCenter();

    std::vector<Vertex> vertexs;
    cocos2d::Color4F color;
};

class Cube3D {

public:
    Cube3D(const cocos2d::Vec3 &center, const cocos2d::Color4F &color);
    virtual ~Cube3D();

    std::vector<Face*> faces;
};

#endif /* Cube3D_h */
