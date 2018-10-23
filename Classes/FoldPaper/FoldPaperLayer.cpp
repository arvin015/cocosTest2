//
//  FoldPaperLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#include "FoldPaperLayer.h"
#include "PolygonView.h"
#include "ui/UIButton.h"

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
            ids(1),
            selectedPolygonView(nullptr),
            rootPolygonView(nullptr),
            doContainerNode(nullptr),
            foldBtn(nullptr) {

    }

    FoldPaperLayer::~FoldPaperLayer() {

    }

    void FoldPaperLayer::onEnter() {
        BaseLayer::onEnter();

        touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
            selectedPolygonView = nullptr;
            return false;
        };
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    }

    void FoldPaperLayer::onExit() {

        if (touchListener != nullptr) {
            Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
        }
        BaseLayer::onExit();
    }

    bool FoldPaperLayer::init() {
        if (!BaseLayer::init()) {
            return false;
        }

        doContainerNode = Node::create();
        doContainerNode->setContentSize(Size(V_WIDTH, V_HEIGHT));
        addChild(doContainerNode);

        for (int i = 0; i < 5; i++) {
            auto polygon = Button::create("mian_button_01_125x54.png");
            polygon->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
            polygon->setPosition(Vec2(20, 650 - i * 60));
            polygon->setTitleText(types[i]);
            polygon->setTitleColor(Color3B::BLACK);
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
                createPolygonView(type, Vec2(V_WIDTH / 2, V_HEIGHT / 2), edge, width, height);
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
                if (selectedPolygonView != nullptr) {
                    selectedPolygonView->updatePolygonColor(colors[i]);
                }
            });
            addChild(b);
        }

        foldBtn = CheckBox::create("paper_btn_fold.png", "paper_btn_unfold.png");
        foldBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        foldBtn->setPosition(Vec2(V_WIDTH / 2, 20));
        foldBtn->setScale(0.5f);
        foldBtn->addEventListener([this](Ref*, CheckBox::EventType) {

        });
        foldBtn->setVisible(false);
        addChild(foldBtn);

        return true;
    }

    void FoldPaperLayer::createPolygonView(PolygonType polygonType, const Vec2 &centerPoint, int edge, float width, float height) {
        PolygonView* polygonView = PolygonView::create();
        switch (polygonType) {
            case SQUARE:
                polygonView->createSquare(centerPoint, width, height);
                break;
            case POLYGON:
                polygonView->createRegularPolygonWithEdgeLength(centerPoint, edge, width);
                break;
        }
        polygonView->setTag(ids);
        polygonView->initView();
        polygonView->setOnTouchEndCallback([this, polygonView]() {
            attachPolygons(polygonView);
        });
        polygonView->setOnSelectCallback([this, polygonView]() {
            selectedPolygonView = polygonView;
        });
        doContainerNode->addChild(polygonView, getMaxOrder());

        polygonViewList.pushBack(polygonView);

        if (selectedPolygonView != nullptr) {
            selectedPolygonView->setPolygonSelectedState(false);
        }
        selectedPolygonView = polygonView;

        ids++;
    }

    void FoldPaperLayer::attachPolygons(PolygonView* polygonView) {

        for (PolygonView* referPolygonView : polygonViewList) {
            if (referPolygonView == polygonView) {
                continue;
            }
            if (polygonView->checkIsCloseEnough(referPolygonView, 15.0f, true)) {
                foldBtn->setVisible(checkCanFold());
                return;
            }
        }

        //未发生吸附
        foldBtn->setVisible(false);
    }

    bool FoldPaperLayer::checkCanFold() {
        initGraph();
        buildGraph();

        if (rootPolygonView == nullptr) return false;

        return getTreeNum(rootPolygonView) == polygonViewList.size();
    }

    void FoldPaperLayer::initGraph() {
        graph.clear();
        for (PolygonView* polygonView : polygonViewList) {
            graph.insert(make_pair(polygonView, Vector<PolygonView*>()));
        }

        for (int i = 0; i < polygonViewList.size(); i++) {
            PolygonView* view = polygonViewList.at(i);
            for (int j = 0; j < i + 1; j++) {
                PolygonView* v = polygonViewList.at(j);
                if (view->checkIsCloseEnough(v, 1.0f)) {
                    graph[view].pushBack(v);
                    graph[v].pushBack(view);
                }
            }
        }
    }

    void FoldPaperLayer::buildGraph() {
        rootPolygonView = getRootPolygonView();
        if (rootPolygonView == nullptr) return;

        map<PolygonView*, bool> visited;

        visited[rootPolygonView] = true;

        queue<PolygonView*> polygonQueue;
        polygonQueue.push(rootPolygonView);

        //清除所有多边形的依赖关系
        for (PolygonView* v : polygonViewList) {
            v->detach();
        }

        while (!polygonQueue.empty()) {

            PolygonView* pv = polygonQueue.front();
            polygonQueue.pop();

            Vector<PolygonView*> childPolygonList = graph[pv];
            for (PolygonView* p : childPolygonList) {
                map<PolygonView*, bool>::iterator it = visited.find(p);
                if (it == visited.end()) {
                    p->attach(pv);
                    visited.insert(make_pair(p, true));

                    polygonQueue.push(p);
                }
            }
        }
    }

    PolygonView* FoldPaperLayer::getRootPolygonView() {
        PolygonView* polygonView = nullptr;
        int maxCount = 0;
        map<PolygonView*, Vector<PolygonView*>>::iterator it;
        for (it = graph.begin(); it != graph.end(); it++) {
            if (it->second.size() > maxCount) {
                polygonView = it->first;
                maxCount = it->second.size();
            }
        }
        return polygonView;
    }

    int FoldPaperLayer::getTreeNum(PolygonView* rootPolygon) {
        int count = 0;
        for (PolygonView* child : rootPolygon->childPolygonViewList) {
            count += getTreeNum(child);
        }
        return count + 1;
    }

    int FoldPaperLayer::getMaxOrder() {
        Vector<Node*> nodeList = doContainerNode->getChildren();
        if (nodeList.size() == 0) return 0;
        Node* node = nodeList.at(doContainerNode->getChildrenCount() - 1); //获取最上面的那个Node
        return node->getLocalZOrder() + 1;
    }
}
