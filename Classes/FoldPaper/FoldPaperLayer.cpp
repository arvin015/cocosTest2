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

USING_NS_CC;
using namespace ui;
using namespace std;

namespace FoldPaper {

    const string types[] = {
            "三角形",
            "正方形",
            "长方形",
            "五边形",
            "六边形"
    };

    const Color4F colors[] = {
            Color4F(76 / 255.0, 198 / 255.0, 198 / 255.0, 1),
            Color4F(1, 1, 0, 1),
            Color4F(1, 0, 0, 1),
            Color4F(1, 0.5f, 0, 1),
            Color4F(0, 0, 1, 1),
            Color4F(0, 1, 0, 1),
    };

    FoldPaperLayer::FoldPaperLayer():
            paperState(PAPER_MAKING),
            makeLayer(nullptr),
            foldLayer(nullptr)
    {

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
        bg->setCameraMask((int)bgcamflag);
        addChild(bg);

        makeLayer = FoldPaperMakeLayer::create();
        makeLayer->setCheckCanFoldCallback([this](bool isOk) {
            foldBtn->setVisible(isOk);
        });
        addChild(makeLayer);

        for (int i = 0; i < 5; i++) {
            auto polygon = Button::create("mian_button_01_125x54.png");
            polygon->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
            polygon->setPosition(Vec2(20, 650 - i * 60));
            polygon->setTitleText(types[i]);
            polygon->setTitleColor(Color3B::WHITE);
            polygon->setTitleFontSize(22);
            polygon->setScale9Enabled(true);
            polygon->setTag(i);
            polygon->addClickEventListener([this](Ref* pSender) {

                foldBtn->setVisible(false);

                auto b = dynamic_cast<Button*>(pSender);
                int tag = b->getTag();

                PolygonType type = POLYGON;
                int edge = tag + 3;
                float width = 120;
                float height = width;
                if (tag == 1 || tag == 2) {
                    type = SQUARE;
                    edge = 4;
                    if (tag == 2) {
                        height = 140;
                    }
                } else if (tag != 0) {
                    edge = tag + 2;
                }
                makeLayer->createPolygonView(type, Vec2(V_WIDTH / 2, V_HEIGHT / 2), edge, width, height);
            });
            addChild(polygon);
        }

        for (int i = 0; i < 6; i++) {
            auto b = Button::create("white.png");
            b->setColor(Color3B(colors[i]));
            b->setContentSize(Size(60, 40));
            b->setScale9Enabled(true);
            b->setPosition(Vec2(980, 80 + i * 50));
            b->addClickEventListener([this, i](Ref* pSender) {
                if (paperState == PAPER_MAKING) {
                    makeLayer->responseColorClick(colors[i]);
                }
            });
            addChild(b);
        }

        foldBtn = CheckBox::create("paper_btn_fold.png", "paper_btn_unfold.png");
        foldBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        foldBtn->setPosition(Vec2(V_WIDTH / 2, 20));
        foldBtn->setScale(0.5f);
        foldBtn->addEventListener([this](Ref* pSender, CheckBox::EventType eventType) {

            makeLayer->setVisible(eventType == CheckBox::EventType::SELECTED ? false : true);
            makeLayer->responseFoldClick(eventType == CheckBox::EventType::SELECTED ? true : false);

            if (foldLayer == nullptr) {
                foldLayer = FoldPaperFoldLayer::create();
                foldLayer->setVisible(false);
                addChild(foldLayer);
            }
            foldLayer->setVisible(eventType == CheckBox::EventType::SELECTED ? true : false);
            foldLayer->responseFoldClick(eventType == CheckBox::EventType::SELECTED ? true : false);

            if (eventType == CheckBox::EventType::SELECTED) {
                vector<Polygon3D*> polygon3DList;
                for (PolygonView* p : makeLayer->polygonViewList) {
                    vector<Vertex> vertexList;
                    for (Vec2 v : p->polygon.vertexList) {
                        v = p->getPolygonViewWorldPoint(v); //转成相对于cocos2d的世界坐标
                        vertexList.push_back(Vertex(Vec3(v.x, v.y, 0), Vec3::ZERO));
                    }
                    Polygon3D* polygon3D = new Polygon3D(vertexList, p->faceType, p->getColor());
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
        delBtn->addClickEventListener([this](Ref* pSender) {
            if (paperState == PAPER_MAKING) {
                makeLayer->responseDelClick();
            }
        });
        addChild(delBtn);

        return true;
    }
}
