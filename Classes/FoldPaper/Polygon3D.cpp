//
// Created by mac_arvin on 2018/11/7.
//

#include "Polygon3D.h"
#include "BasicData.h"

USING_NS_CC;
using namespace std;

namespace FoldPaper {

    Polygon3D::Polygon3D()
    : polygonColor(Color4F(76 / 255.0, 198 / 255.0, 198 / 255.0, 1))
    , targetAngle(3.14 / 2) //HALF_PI
    , rotatedTotal(0)
    , faceType(FaceType::FaceTypeUnknown) {

    }

    Polygon3D::Polygon3D(const vector<Vertex> &vertexList, int faceType, const Color4F &polygonColor)
    : Polygon3D() {

        this->vertexList = vertexList;
        this->faceType = faceType;
        this->polygonColor = polygonColor;
    }

    Polygon3D::~Polygon3D() {

    }

    Vec3 Polygon3D::getCenter() {
        Vec3 center;
        for (Vertex vertex : vertexList) {
            center += vertex.position;
        }
        center = center / vertexList.size();
        return center;
    }

    Vec3 Polygon3D::getNormal() {
        Vec3 p0 = vertexList[1].position - vertexList[0].position;
        Vec3 p1 = vertexList[2].position - vertexList[0].position;
        p0.cross(p1);
        p0.normalize();
        return p0.getNormalized();
    }

    Vec3 Polygon3D::closestPointToLine(const Vec3 &pt, const Vec3 &p0, const Vec3 &p1) {
        Vec3 ab = p1 - p0;
        float t = (pt - p0).dot(ab) / ab.dot(ab);
        return p0 + t * ab;
    }

    void Polygon3D::rotateAround(const cocos2d::Vec3 &p0, const cocos2d::Vec3 &p1, float angle) {
        for (int i = 0; i < vertexList.size(); i++) {
            Vec3 pt = vertexList[i].position;

            Vec3 o = closestPointToLine(pt, p0, p1);
            float r = (pt - o).length();

            Vec3 u = pt - o;
            u.normalize();
            u = u.getNormalized();

            Vec3 v = p0 - p1;
            v.cross(u);
            v.normalize();
            v = v.getNormalized();

            Vec3 out = o + r * u * cos(angle) + r * v * sin(angle);
            vertexList[i].position = out;
        }
    }

    void Polygon3D::recursiveRotate(const Vec3 &p0, const Vec3 &p1, float angle, bool recursive) {
        rotateAround(p0, p1, angle);

        if (recursive) {
            for (Polygon3D* p : child) {
                p->recursiveRotate(p0, p1, angle, recursive);
            }
        }
    }

    bool Polygon3D::isStickTogether(const Polygon3D* other, int* axisIds) {

        for (int i = 0; i < vertexList.size(); i++) {

            int i0 = i;
            int i1 = i == vertexList.size() - 1 ? 0 : i + 1;

            Vec3 p0 = vertexList.at(i0).position;
            Vec3 p1 = vertexList.at(i1).position;

            for (int j = 0; j < other->vertexList.size(); j++) {

                int j0 = j;
                int j1 = j == other->vertexList.size() - 1 ? 0 : j + 1;

                Vec3 p2 = other->vertexList.at(j0).position;
                Vec3 p3 = other->vertexList.at(j1).position;

                Vec3 EPSILON = Vec3(0.1f, 0.1f, 0.1f);

                if ((Vec3(fabsf((p0 - p2).x), fabsf((p0 - p2).y), fabsf((p0 - p2).z)) < EPSILON &&
                        Vec3(fabsf((p1 - p3).x), fabsf((p1 - p3).y), fabsf((p1 - p3).z)) < EPSILON) ||
                        (Vec3(fabsf((p0 - p3).x), fabsf((p0 - p3).y), fabsf((p0 - p3).z)) < EPSILON &&
                                Vec3(fabsf((p1 - p2).x), fabsf((p1 - p2).y), fabsf((p1 - p2).z)) < EPSILON)) {

                    if (axisIds != nullptr) {
                        axisIds[0] = i0;
                        axisIds[1] = i1;
                        axisIds[2] = j0;
                        axisIds[3] = j1;
                    }
                    return true;
                }
            }
        }

        return false;
    }

    int Polygon3D::getDepth() {
        int count = 0;
        Polygon3D* p = this;
        while (p->parent != nullptr) {
            count++;
            p = p->parent;
        }
        return count;
    }
}
