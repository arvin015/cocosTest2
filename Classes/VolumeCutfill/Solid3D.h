//
//  Solid3D.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/15.
//

#ifndef Solid3D_h
#define Solid3D_h

#include <stdio.h>
#include "cocos2d.h"
#include "Geom3D.h"
#include "VolumeQuestion.h"
#include "json/document.h"

const float Volume_MoveByValue = 0.08f;
const float Volume_Duration = 0.3f;

class CC3DLayer;
class cG3DefModelGen;

class Solid3D : public cocos2d::Sprite3D {

public:

    enum TRANS_DIRECTION {
        HOR,
        VER
    };

    Solid3D();
    virtual ~Solid3D();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();
    CREATE_FUNC(Solid3D);

    void toJson(rapidjson::Document &doc);
    void fromJson(const rapidjson::Value &json);

    void set3DLayer(CC3DLayer* cc3DLayer);
    void setData(const std::vector<VertexInfo> &vecs);

    void showModel();

    /**
     * 获取最近的点
     */
    bool getMinDisPoint(const cocos2d::Vec2 &point, const cocos2d::Vec2 &startPoint, float maxDis, VertexInfo &rs, float &dist);

    /**
     * 是否可切割
     */
    bool checkCanCut(const cocos2d::Vec2 &p1, const cocos2d::Vec2 &p2);

    /**
     * 分割
     */
    void cutPolygon(const cocos2d::Vec2 &p1, const cocos2d::Vec2 &p2, std::vector<VertexInfo> &leftVecList, std::vector<VertexInfo> &rightVecList);

    /**
     * 播放未参与分割块移位动画
     */
    void playTransAnim(float duration, const cocos2d::Vec2 &p1, const cocos2d::Vec2 &p2, const TRANS_DIRECTION &direction);

    /**
     * 播放移位动画
     */
    void playTranslateAnim(float duration, const cocos2d::Vec3 &offset);

    /**
     * 清除---Test
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
    void vertex3dTo2d();

    /**
     * 获取2d中心点坐标
     */
    cocos2d::Vec2 getCenterPoint();

    /**
     * 绘制2D
     */
    void draw2D();

private:
    CC3DLayer* cc3DLayer;
    Geom3D geom3D;

    cocos2d::Sprite3D* topSp3d;
    cocos2d::Sprite3D* sideSp3d;
    cocos2d::Sprite3D* bottomSp3d;
    std::vector<cocos2d::Sprite3D*> spEdges;

    cocos2d::DrawNode* dotDrawNode;

    cocos2d::Vec3 offset; //动画位移值
    std::vector<VertexInfo> top2dVecs; //屏幕上2d坐标集
};

#endif /* Solid3D_h */
