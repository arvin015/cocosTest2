//
//  CubeSprite3D.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/1/22.
//

#ifndef CubeSprite3D_h
#define CubeSprite3D_h

#include <stdio.h>
#include "cocos2d.h"
#include "DrawNode3D.h"

class CubeSprite3D : public cocos2d::Sprite3D {
public:

    CubeSprite3D();
    CubeSprite3D(const std::string &modelName);
    virtual ~CubeSprite3D();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    static CubeSprite3D* create(std::string modelName = "");

    void setBorderColor(const cocos2d::Color4F &borderColor);

    void initVertexs();

public:

    void drawCube();

private:

    cocos2d::DrawNode3D* drawNode3D;
    cocos2d::Color4F borderColor;

    std::string modelName;
    std::vector<cocos2d::Vec3> vertexList;
};

#endif /* CubeSprite3D_h */
