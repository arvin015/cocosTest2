//
// Created by mac_arvin on 2018/11/7.
//

#include "Polygon3D.h"
#include "BasicData.h"
#include "TriangleTriangle.h"

USING_NS_CC;
using namespace std;

namespace FoldPaper {

    inline Rect3D getBoundBox(const Vec3 &a, const Vec3 &b, const Vec3 &c) {
        float inf = std::numeric_limits<float>::infinity();
        Vec3 minPt(inf, inf, inf);
        Vec3 maxPt(-inf, -inf, -inf);

        Vec3 pts[3] = { a, b, c };
        for (int j = 0; j < 3; j++) {
            float x = pts[j].x;
            float y = pts[j].y;
            float z = pts[j].z;

            if (x < minPt.x) minPt.x = x;
            if (y < minPt.y) minPt.y = y;
            if (z < minPt.z) minPt.z = z;

            if (x > maxPt.x) maxPt.x = x;
            if (y > maxPt.y) maxPt.y = y;
            if (z > maxPt.z) maxPt.z = z;
        }
        return Rect3D(maxPt, minPt);
    }

    inline bool isNotOverlapped(const Rect3D &rect1, const Rect3D &rect2) {
        if (rect1.minPoint.x > rect2.maxPoint.x) return true;
        if (rect1.minPoint.y > rect2.maxPoint.y) return true;
        if (rect1.minPoint.z > rect2.maxPoint.z) return true;

        if (rect2.minPoint.x > rect1.maxPoint.x) return true;
        if (rect2.minPoint.y > rect1.maxPoint.y) return true;
        if (rect2.minPoint.z > rect1.maxPoint.z) return true;
        return false;
    }

    Polygon3D::Polygon3D()
    : polygonColor(Color4F(76 / 255.0, 198 / 255.0, 198 / 255.0, 1))
    , targetAngle(3.14 / 2) //HALF_PI
    , rotatedTotal(0)
    , faceType(FaceType::FaceTypeUnknown)
    , textureId(0)
    , parent(nullptr) {

    }

    Polygon3D::Polygon3D(const vector<Vertex> &vertexList, int faceType, const Color4F &polygonColor, unsigned int textureId)
    : Polygon3D() {
        this->vertexList = vertexList;
        this->faceType = faceType;
        this->polygonColor = polygonColor;
        this->textureId = textureId;
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

            Vec3 u = (pt - o).getNormalized();

            Vec3 v = p0 - p1;
            v.cross(u);
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

    bool Polygon3D::isPolygonsIntersected(Polygon3D* otherPolygon) {
        int count = getVertexCount();
        int otherCount = otherPolygon->getVertexCount();
        if (count < 3 || otherCount < 3) return false;

        if ((getCenter() - otherPolygon->getCenter()).length() < 0.1f) {
            float dot = getNormal().dot(otherPolygon->getNormal());
            if (fabs(dot) > 0.9998f) return true;
        }

        const float scale = 0.98f;
        for (size_t j = 2; j < count; j++) {
            Vec3 v0 = vertexList[0].position;
            Vec3 v1 = vertexList[j - 1].position;
            Vec3 v2 = vertexList[j].position;
            Vec3 center1 = (v0 + v1 + v2) / 3.0f;
            // Make the triangle smaller to avoid boundary case
            v0 = center1 + scale * (v0 - center1).length() * (v0 - center1).getNormalized();
            v1 = center1 + scale * (v1 - center1).length() * (v1 - center1).getNormalized();
            v2 = center1 + scale * (v2 - center1).length() * (v2 - center1).getNormalized();
            for (size_t k = 2; k < otherCount; k++) {
                Vec3 p0 = otherPolygon->vertexList[0].position;
                Vec3 p1 = otherPolygon->vertexList[k - 1].position;
                Vec3 p2 = otherPolygon->vertexList[k].position;
                Vec3 center0 = (p0 + p1 + p2) / 3.0f;
                p0 = center0 + scale * (p0 - center0).length() * (p0 - center0).getNormalized();
                p1 = center0 + scale * (p1 - center0).length() * (p1 - center0).getNormalized();
                p2 = center0 + scale * (p2 - center0).length() * (p2 - center0).getNormalized();

                // The Triangle to Triangle test fail in some case, we use the bounding box test to help
                Rect3D box0 = getBoundBox(v0, v1, v2);
                Rect3D box1 = getBoundBox(p0, p1, p2);
                if (isNotOverlapped(box0, box1)) continue;
                if (IsTrianglesIntersected(v0, v1, v2, p0, p1, p2)) return true;
            }
        }
        return false;
    }

    void Polygon3D::backup(bool recursively) {
        backupVertexList.clear();
        backupVertexList = vertexList;
        if (recursively) {
            for (int i = 0; i < child.size(); i++) {
                child[i]->backup(recursively);
            }
        }
    }

    void Polygon3D::restore(bool recursively) {
        if (backupVertexList.size() != vertexList.size()) return;

        for (int i = 0; i < vertexList.size(); i++) {
            vertexList[i] = backupVertexList[i];
        }

        if (recursively) {
            for (int i = 0; i < child.size(); i++) {
                child[i]->restore(recursively);
            }
        }
    }

}
