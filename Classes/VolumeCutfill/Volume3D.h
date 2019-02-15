//
//  Volume3D.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/15.
//

#ifndef Volume3D_h
#define Volume3D_h

#include <stdio.h>
#include "cocos2d.h"
#include "eGeom3D.h"

class CC3DLayer;

class Volume3D : public cocos2d::Ref {

public:

    Volume3D();
    virtual ~Volume3D();

    void setData(CC3DLayer* cc3DLayer, std::vector<cocos2d::Vec2> vecs);

    void showModel();

    /**
     * 获取最近的点
     */
    void findMinDisPoint(const cocos2d::Vec2 &point, const cocos2d::Vec2 &startPoint, cocos2d::Vec2 &rsPoint, float &dis);

    /**
     * 分割
     */
    void cutPolygon(const cocos2d::Vec2 &p1, const cocos2d::Vec2 &p2, std::vector<cocos2d::Vec2> &leftVecList, std::vector<cocos2d::Vec2> &rightVecList);

    /**
     * 播放移位动画
     */
    void playTranslateAnim(float duration, const cocos2d::Vec3 &offset);

    /**
     * 清除
     */
    void clear();

private:

    /**
     * 加载面
     */
    void loadFaces();

    /**
     * 加载描边
     */
    void loadEdges();

    /**
     * 创建描边Sprite3D
     */
    Sprite3D* makeEdgeSprite(const cocos2d::Vec3 &v1, const cocos2d::Vec3 &v2, cocos2d::Node *cc3dlayer);

    /**
     * 世界坐标转成屏幕坐标
     */
    void vector3dTo2d();

    /**
     * 绘制2D
     */
    void draw2D();

private:
    CC3DLayer* cc3DLayer;
    eGeom3D geom3D;

    cocos2d::Sprite3D* volume3d;
    cocos2d::Sprite3D* topSp3d;
    cocos2d::Sprite3D* sideSp3d;
    cocos2d::Sprite3D* bottomSp3d;
    std::vector<cocos2d::Sprite3D*> spEdges;

    cocos2d::DrawNode* dotDrawNode;

    cocos2d::Vec3 offset; //动画位移值
    std::vector<cocos2d::Vec2> top2dVecs;
};

#endif /* Volume3D_h */
