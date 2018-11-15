//
//  FoldPaperLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#include "FoldPaperLayer.h"
#include "ui/UIButton.h"
#include "FoldPaperMakeLayer.h"
#include "FoldPaperFoldLayer.h"
#include "BasicData.h"

USING_NS_CC;
using namespace ui;
using namespace std;

namespace FoldPaper {

    const string types[] = {
            "正方形",
            "长方形"
    };

    const string cubes[] = {
            "正方体",
            "长方体",
            "三面柱",
            "五面柱",
            "六面柱"
    };

    const Color4F colors[] = {
            Color4F(76 / 255.0, 198 / 255.0, 198 / 255.0, 1),
            Color4F(1, 1, 0, 1),
            Color4F(1, 0, 0, 1),
            Color4F(1, 0.5f, 0, 1),
            Color4F(0, 0, 1, 1),
            Color4F(0, 1, 0, 1),
    };

    const string textureNames[] = {
            "小乌龟",
            "小兔子",
            "小狗狗"
    };

    const string textureImgs[] = {
            "paper_img_animal_tortoise.png",
            "paper_img_animal_rabbit.png",
            "paper_img_animal_dog.png"
    };

    FoldPaperLayer::FoldPaperLayer() :
            paperState(PAPER_MAKING),
            makeLayer(nullptr),
            foldLayer(nullptr),
            shapeIndex(-1) {

    }

    FoldPaperLayer::~FoldPaperLayer() {

    }

    void FoldPaperLayer::onEnter() {
        BaseLayer::onEnter();
    }

    void FoldPaperLayer::onExit() {
        BaseLayer::onExit();
    }

    bool FoldPaperLayer::init() {
        if (!BaseLayer::init()) {
            return false;
        }

        const CameraFlag bgcamflag = CameraFlag::USER2;
        auto ca = Camera::create();
        ca->setDepth(-1);
        ca->setCameraFlag(bgcamflag);
        addChild(ca);

        auto bg = Sprite::create("white.png");
        bg->setScaleX(V_WIDTH);
        bg->setScaleY(V_HEIGHT);
        bg->setCameraMask((int) bgcamflag);
        addChild(bg);

        makeLayer = FoldPaperMakeLayer::create();
        makeLayer->setCheckCanFoldCallback([this](bool isOk) {
            foldBtn->setVisible(isOk);
        });
        addChild(makeLayer);

        foldLayer = FoldPaperFoldLayer::create();
        foldLayer->setVisible(false);
        addChild(foldLayer);

        for (int i = 0; i < 2; i++) {
            auto polygon = Button::create("mian_button_01_125x54.png");
            polygon->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
            polygon->setPosition(Vec2(20, 650 - i * 60));
            polygon->setTitleText(types[i]);
            polygon->setTitleColor(Color3B::WHITE);
            polygon->setTitleFontSize(22);
            polygon->setScale9Enabled(true);
            polygon->setTag(i);
            polygon->addClickEventListener([this](Ref *pSender) {

                if (shapeIndex != -1) return;

                foldBtn->setVisible(false);

                auto b = dynamic_cast<Button *>(pSender);
                int tag = b->getTag();

                int edge = 4;
                float width = 120;
                float height = 120;

                if (tag == 1) {
                    height = 120;
                    width = PHI * 120;
                }
                makeLayer->createPolygonView(tag == 0 ? SQUARE : RECTANGLE,
                                             Vec2(V_WIDTH / 2, V_HEIGHT / 2), edge, width, height);
            });
            addChild(polygon);
        }

        for (int i = 0; i < 5; i++) {
            auto p = Button::create("mian_button_01_125x54.png");
            p->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
            p->setPosition(Vec2(20, 500 - i * 60));
            p->setTitleText(cubes[i]);
            p->setTitleColor(Color3B::WHITE);
            p->setTitleFontSize(22);
            p->setScale9Enabled(true);
            p->setTag(i);
            p->addClickEventListener([this, i](Ref *pSender) {
                selectShape(i);
            });
            addChild(p);
        }

        for (int i = 0; i < 6; i++) {
            auto b = Button::create("white.png");
            b->setColor(Color3B(colors[i]));
            b->setContentSize(Size(60, 40));
            b->setScale9Enabled(true);
            b->setPosition(Vec2(980, 80 + i * 50));
            b->addClickEventListener([this, i](Ref *pSender) {
                if (paperState == PAPER_MAKING) {
                    makeLayer->responseColorClick(colors[i]);
                }
            });
            addChild(b);
        }

        for (int i = 0; i < 3; i++) {
            auto btn = Button::create("mian_button_01_125x54.png");
            btn->setTitleText(textureNames[i]);
            btn->setTitleColor(Color3B::WHITE);
            btn->setTitleFontSize(22);
            btn->setName(textureImgs[i]);
            btn->setPosition(Vec2(940, 650 - i * 60));
            btn->addClickEventListener([this, i](Ref *pSender) {
                if (paperState == PAPER_MAKING) {
                    makeLayer->responseTextureClick(textureImgs[i]);
                }
            });
            addChild(btn);
        }

        foldBtn = CheckBox::create("paper_btn_fold.png", "paper_btn_unfold.png");
        foldBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        foldBtn->setPosition(Vec2(V_WIDTH / 2, 20));
        foldBtn->setScale(0.5f);
        foldBtn->addEventListener([this](Ref *pSender, CheckBox::EventType eventType) {

            makeLayer->setVisible(eventType == CheckBox::EventType::SELECTED ? false : true);
            makeLayer->responseFoldClick(eventType == CheckBox::EventType::SELECTED ? true : false);

            foldLayer->setVisible(eventType == CheckBox::EventType::SELECTED ? true : false);
            foldLayer->responseFoldClick(eventType == CheckBox::EventType::SELECTED ? true : false);

            if (eventType == CheckBox::EventType::SELECTED) {
                vector<Polygon3D *> polygon3DList;
                for (PolygonView *p : makeLayer->polygonViewList) {
                    vector<Vertex> vertexList;
                    for (Vec2 v : p->polygon.vertexList) {
                        v = p->getPolygonViewWorldPoint(v); //转成相对于cocos2d的世界坐标
                        v = v / 60; //转成相对于3D坐标
                        vertexList.push_back(Vertex(Vec3(v.x, v.y, 0), Vec3::ZERO));
                    }
                    Polygon3D *polygon3D = new Polygon3D(vertexList, p->faceType, p->getColor(),
                                                         p->getTextureId());
                    polygon3DList.push_back(polygon3D);
                }

                foldLayer->setData(polygon3DList);
                paperState = PAPER_FOLDING;

            } else if (eventType == CheckBox::EventType::UNSELECTED) {
                paperState = PAPER_MAKING;
            }
        });
        foldBtn->setVisible(false);
        addChild(foldBtn);

        delBtn = Button::create("mian_button_01_125x54.png");
        delBtn->setAnchorPoint(Vec2::ZERO);
        delBtn->setPosition(Vec2(20, 20));
        delBtn->setScale9Enabled(true);
        delBtn->setTitleFontSize(22);
        delBtn->setTitleText("删除");
        delBtn->setTitleColor(Color3B::WHITE);
        delBtn->addClickEventListener([this](Ref *pSender) {
            if (paperState == PAPER_MAKING) {
                makeLayer->responseDelClick();
            }
        });
        addChild(delBtn);

        return true;
    }

    void FoldPaperLayer::selectShape(int shapeIndex) {

        if (paperState == PAPER_FOLDING) return;
        if (this->shapeIndex == shapeIndex) return;

        reset();

        this->shapeIndex = shapeIndex;

        float edgeLength = 100.0f; //边长

        switch (shapeIndex) {
            case 0:
            {
                makeLayer->createPolygonViewFromCube(edgeLength);
                break;
            }
            case 1:
            {
                makeLayer->createPolygonViewFromPrism(4, edgeLength);
                break;
            }
            case 2:
            {
                makeLayer->createPolygonViewFromPrism(3, edgeLength);
                break;
            }
            case 3:
            {
                makeLayer->createPolygonViewFromPrism(5, edgeLength);
                break;
            }
            case 4:
            {
                makeLayer->createPolygonViewFromPrism(6, edgeLength);
                break;
            }
        }
    }

    void FoldPaperLayer::reset() {
        if (paperState == PAPER_MAKING) {
            shapeIndex = -1;
            foldBtn->setVisible(false);
            makeLayer->responseResetClick();
        } else if (paperState == PAPER_FOLDING) {

        }
    }
}
