//
// Created by mac_arvin on 2018/11/7.
//

#ifndef PROJ_ANDROID_STUDIO_POLYGON3D_H
#define PROJ_ANDROID_STUDIO_POLYGON3D_H

#include "cocos2d.h"

namespace FoldPaper {

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

    class Rect3D {
    public:
        Rect3D() {};
        Rect3D(const cocos2d::Vec3 &maxPoint, const cocos2d::Vec3 &minPoint) {
            this->maxPoint = maxPoint;
            this->minPoint = minPoint;
        }
        ~Rect3D() {};

        cocos2d::Vec3 maxPoint;
        cocos2d::Vec3 minPoint;
    };

    class Polygon3D : public cocos2d::Ref {

    public:
        Polygon3D();
        Polygon3D(const std::vector<Vertex> &vertexList, int faceType, const cocos2d::Color4F &polygonColor,
                  unsigned int textureId);
        ~Polygon3D();

        /**
         * 获取顶点个数
         */
        int getVertexCount() {
            return (int)vertexList.size();
        }

        /**
         * 获取指定顶点
         */
        Vertex getVertexById(int id) {
            return vertexList.at(id);
        }

        /**
         * 获取中心点坐标
         * @return
         */
        cocos2d::Vec3 getCenter();

        /**
         * 获取标准化坐标
         * @return
         */
        cocos2d::Vec3 getNormal();

        cocos2d::Vec3 closestPointToLine(const cocos2d::Vec3 &pt, const cocos2d::Vec3 &p0, const cocos2d::Vec3 &p1);

        /**
         * 多边形旋转
         * @param p0
         * @param p1
         * @param angle
         */
        void rotateAround(const cocos2d::Vec3 &p0, const cocos2d::Vec3 &p1, float angle);

        /**
         * 多边形联动旋转
         * @param p0
         * @param p1
         * @param angle
         * @param recursive 是否让子联动旋转
         */
        void recursiveRotate(const cocos2d::Vec3 &p0, const cocos2d::Vec3 &p1, float angle, bool recursive = true);

        /**
         * 两个多边形是否吸附在一起，需满足有两个顶点一样
         * @param other
         * @return
         */
        bool isStickTogether(const Polygon3D* other, int* axisIds);

        /**
         * 获取该多边形顶上有多少父层
         * @return
         */
        int getDepth();

        /**
         * 两个多边形是否相交
         * @param poly0
         * @param poly1
         * @return
         */
        bool isPolygonsIntersected(Polygon3D* otherPolygon);

        /**
         * 备份还原
         * @param recursively
         */
        void backup(bool recursively = false);
        void restore(bool recursively = false);

    public:
        std::vector<Vertex> vertexList; //点集合
        std::vector<Vertex> backupVertexList; //备份的顶点集合
        cocos2d::Color4F polygonColor; //多边形颜色
        unsigned int textureId; //纹理ID
        int faceType;

        //for fold
        float targetAngle; //总需旋转角度
        std::vector<int> foldAxisIds; //折叠边顶点
        float rotatedTotal; //已旋转角度

        Polygon3D* parent;
        std::vector<Polygon3D*> child;
    };
}

#endif //PROJ_ANDROID_STUDIO_POLYGON3D_H
