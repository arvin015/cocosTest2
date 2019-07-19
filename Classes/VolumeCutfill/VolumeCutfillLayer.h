//
//  VolumeCutfillLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/12.
//

#ifndef VolumeCutfillLayer_h
#define VolumeCutfillLayer_h

#include <stdio.h>
#include "BaseLayer.h"
#include "cMultiTouch.h"
#include "CC3DLayer.h"
#include "ui/UIButton.h"
#include "ui/UIRadioButton.h"
#include "ui/UIText.h"
#include "CutLayer.h"
#include "FillLayer.h"

class VolumeQuestion;

class VolumeCutfillLayer : public BaseLayer {

public:

    enum WorkType {
        NONE,
        CUT,
        FILL
    };

    VolumeCutfillLayer();
    virtual ~VolumeCutfillLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(VolumeCutfillLayer);

    virtual bool shouldWhiteBg() {return false;};

    virtual std::string toJSON();
    virtual void fromJSON(const std::string &json);

    void setWorkType(WorkType workType, bool isLoadQuestion);

    void reset();

    void saveRecord();

    void loadRecord();

private:

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    void loadQuestion(int qId);

    VolumeQuestion* getQuestionById(int qId);

private:

    cocos2d::EventListenerTouchOneByOne* touchListener;
    CC3DLayer* cc3DLayer;
    cocos2d::DirectionLight* light;
    cocos2d::Vec3 camtarget, camoffset;
    cocos2d::Quaternion camquat;
    cMultiTouch multitouch;

    std::vector<VolumeQuestion*> questionList;

    BaseCutfillLayer* currentLayer;
    CutLayer* cutLayer;
    FillLayer* fillLayer;

    int currentQId;

    WorkType workTypeQ1;
    WorkType workTypeQ2;

    cocos2d::ui::Button* cutBtn;
    cocos2d::ui::Button* fillBtn;
    cocos2d::ui::Button* resetBtn;

    //test
    enum Type3D {
        NON,
        CUBE,
        CONE,
        PYRAMID,
        SPHERE,
        CYLINDER
    };
    Type3D curType3D = NON;

    cocos2d::ui::Button* cubeBtn;
    cocos2d::ui::Button* coneBtn;
    cocos2d::ui::Button* sphereBtn;
    cocos2d::ui::Button* pyramidBtn;
    cocos2d::ui::Button* cylinderBtn;
    cocos2d::Sprite3D* cubeSp3d;
    cocos2d::Sprite3D* coneSp3d;
    cocos2d::Sprite3D* pyramidSp3d;
    cocos2d::Sprite3D* sphereSp3d;
    cocos2d::Sprite3D* cylinderSp3d;
    cocos2d::ui::Text* pyramidNumText;
    cocos2d::ui::Button* curBtn;
    cocos2d::Sprite3D* curSp3d;
    //end
};

#endif /* VolumeCutfillLayer_h */
