//
// Created by mac_arvin on 2018/11/9.
//

#ifndef PROJ_ANDROID_STUDIO_BASICDATA_H
#define PROJ_ANDROID_STUDIO_BASICDATA_H

#endif //PROJ_ANDROID_STUDIO_BASICDATA_H

namespace FoldPaper {

    enum FaceType {
        FaceTypeUnknown = 0,
        FaceTypeBase,
        FaceTypeLateralFace
    };

    enum PolygonType {
        TRIANGLE, //等腰三角形
        SQUARE, //正方形
        RECTANGLE, //长方形
        POLYGON, //规则的多边形
        CIRCLE, //圆
        ARC, //圆弧
    };

    enum ShapeType {
        NORMAL,
        CYLINDER, //圆柱
        CIRCULAR_CONE, //圆锥
        THREE_CONE, //正三角锥
        FOUR_CONE, //正四角锥
        FIVE_CONE, //正五角锥
        SIX_CONE, //正六角锥
    };
}