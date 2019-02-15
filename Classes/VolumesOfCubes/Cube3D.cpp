//
//  Cube3D.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/1/15.
//

#include "Cube3D.h"

USING_NS_CC;

Face::Face(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4, const Color4F &color) {

    this->color = color;

    vertexs.push_back(Vertex(p1, Vec2::ZERO));
    vertexs.push_back(Vertex(p2, Vec2::ZERO));
    vertexs.push_back(Vertex(p3, Vec2::ZERO));
    vertexs.push_back(Vertex(p4, Vec2::ZERO));
}

Face::~Face() {

}

Vec3 Face::getNormal() {
    Vec3 p0 = vertexs[1].position - vertexs[0].position;
    Vec3 p1 = vertexs[2].position - vertexs[0].position;
    p0.cross(p1);
    return p0.getNormalized();
}

Vec3 Face::getCenter() {
    Vec3 center;
    for (Vertex v : vertexs) {
        center += v.position;
    }
    return center / vertexs.size();
}

Cube3D::Cube3D(const Vec3 &center, const Color4F &color) {

    float x = center.x;
    float y = center.y;
    float z = center.z;

    faces.push_back(new Face(Vec3(x - 0.5, y + 0.5, z + 0.5), Vec3(x + 0.5, y + 0.5, z + 0.5),
                                    Vec3(x + 0.5, y - 0.5, z + 0.5), Vec3(x - 0.5, y - 0.5, z + 0.5), color)); //后
    faces.push_back(new Face(Vec3(x - 0.5, y + 0.5, z - 0.5), Vec3(x + 0.5, y + 0.5, z - 0.5),
                                    Vec3(x + 0.5, y - 0.5, z - 0.5), Vec3(x - 0.5, y - 0.5, z - 0.5), color)); //前
    faces.push_back(new Face(Vec3(x - 0.5, y + 0.5, z + 0.5), Vec3(x - 0.5, y + 0.5, z - 0.5),
                                    Vec3(x - 0.5, y - 0.5, z - 0.5), Vec3(x - 0.5, y - 0.5, z + 0.5), color)); //左
    faces.push_back(new Face(Vec3(x + 0.5, y + 0.5, z - 0.5), Vec3(x + 0.5, y + 0.5, z + 0.5),
                                    Vec3(x + 0.5, y - 0.5, z + 0.5), Vec3(x + 0.5, y - 0.5, z - 0.5), color)); //右
    faces.push_back(new Face(Vec3(x - 0.5, y - 0.5, z + 0.5), Vec3(x + 0.5, y - 0.5, z + 0.5),
                                    Vec3(x + 0.5, y - 0.5, z - 0.5), Vec3(x - 0.5, y - 0.5, z - 0.5), color)); //下
    faces.push_back(new Face(Vec3(x - 0.5, y + 0.5, z + 0.5), Vec3(x + 0.5, y + 0.5, z + 0.5),
                                    Vec3(x + 0.5, y + 0.5, z - 0.5), Vec3(x - 0.5, y + 0.5, z - 0.5), color)); //上
}

Cube3D::~Cube3D() {

}
