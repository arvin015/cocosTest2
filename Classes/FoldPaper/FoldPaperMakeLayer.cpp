//
// Created by mac_arvin on 2018/11/8.
//

#include "FoldPaperMakeLayer.h"

USING_NS_CC;
using namespace std;

namespace FoldPaper {

    FoldPaperMakeLayer::FoldPaperMakeLayer()
    : ids(1),
      selectedPolygonView(nullptr),
      rootPolygonView(nullptr),
      doContainerNode(nullptr),
      isTouchEnabled(true),
      checkCanFoldCallback(nullptr) {

    }

    FoldPaperMakeLayer::~FoldPaperMakeLayer() {

    }

    void FoldPaperMakeLayer::onEnter() {
        Layer::onEnter();

        touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = CC_CALLBACK_2(FoldPaperMakeLayer::onTouchBegan, this);
        touchListener->onTouchMoved = CC_CALLBACK_2(FoldPaperMakeLayer::onTouchMoved, this);
        touchListener->onTouchEnded = CC_CALLBACK_2(FoldPaperMakeLayer::onTouchEnded, this);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    }

    void FoldPaperMakeLayer::onExit() {

        if (touchListener != nullptr) {
            Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
        }

        Layer::onExit();
    }

    bool FoldPaperMakeLayer::init() {
        if (!Layer::init()) {
            return false;
        }

        doContainerNode = Node::create();
        doContainerNode->setContentSize(Size(1024, 768));
        addChild(doContainerNode);

        return true;
    }

    bool FoldPaperMakeLayer::onTouchBegan(Touch* touch, Event* event) {
        if (!isTouchEnabled) {
            return false;
        }
        selectedPolygonView = nullptr;
        return true;
    }

    void FoldPaperMakeLayer::onTouchMoved(Touch* touch, Event* event) {
        updateAllPolygonsPosition(touch->getDelta());
    }

    void FoldPaperMakeLayer::onTouchEnded(Touch* touch, Event* event) {
    }

    void FoldPaperMakeLayer::createPolygonView(PolygonType polygonType, const Vec2 &centerPoint, int edge, float width, float height) {
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

    void FoldPaperMakeLayer::responseColorClick(const cocos2d::Color4F &color) {
        if (selectedPolygonView != nullptr) {
            selectedPolygonView->updatePolygonColor(color);
        }
    }

    void FoldPaperMakeLayer::responseFoldClick(bool isChecked) {
        isTouchEnabled = !isChecked;
        setPolygonsTouchEnabled(!isChecked);
    }

    void FoldPaperMakeLayer::responseDelClick() {
        if (selectedPolygonView != nullptr) {
            selectedPolygonView->removeFromParent();
            polygonViewList.eraseObject(selectedPolygonView);
            selectedPolygonView = nullptr;

            if (checkCanFoldCallback != nullptr) {
                checkCanFoldCallback(checkCanFold());
            }
        }
    }

    void FoldPaperMakeLayer::updateAllPolygonsPosition(const cocos2d::Vec2 &delta) {
        for (PolygonView* p : polygonViewList) {
            p->movePolygon(delta);
        }
    }

    void FoldPaperMakeLayer::attachPolygons(PolygonView* polygonView) {

        for (PolygonView* referPolygonView : polygonViewList) {
            if (referPolygonView == polygonView) {
                continue;
            }
            if (polygonView->checkIsCloseEnough(referPolygonView, 15.0f, true)) {
                if (checkCanFoldCallback != nullptr) {
                    checkCanFoldCallback(checkCanFold());
                }
                return;
            }
        }

        //未发生吸附
        if (checkCanFoldCallback != nullptr) {
            checkCanFoldCallback(false);
        }
    }

    bool FoldPaperMakeLayer::checkCanFold() {

        if (isExitOverlap()) return false;

        initGraph();
        buildGraph();

        if (rootPolygonView == nullptr) return false;

        return getTreeNum(rootPolygonView) == polygonViewList.size();
    }

    bool FoldPaperMakeLayer::isExitOverlap() {
        for (int i = 0; i < polygonViewList.size(); i++) {
            PolygonView* polygonView = polygonViewList.at(i);
            for (int j = i + 1; j < polygonViewList.size(); j++) {
                PolygonView* p = polygonViewList.at(j);
                if (polygonView->checkIsOverlap(p)) {
                    return true;
                }
            }
        }
        return false;
    }

    void FoldPaperMakeLayer::initGraph() {
        graph.clear();
        for (PolygonView* polygonView : polygonViewList) {
            graph.insert(make_pair(polygonView, Vector<PolygonView*>()));
        }

        for (int i = 0; i < polygonViewList.size(); i++) {
            PolygonView* view = polygonViewList.at(i);
            for (int j = i + 1; j < polygonViewList.size(); j++) {
                PolygonView* v = polygonViewList.at(j);
                if (view->checkIsCloseEnough(v, 1.0f, nullptr)) {
                    graph[view].pushBack(v);
                    graph[v].pushBack(view);
                }
            }
        }
    }

    void FoldPaperMakeLayer::buildGraph() {
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
            for (PolygonView* childPv : childPolygonList) {
                map<PolygonView*, bool>::iterator it = visited.find(childPv);
                if (it == visited.end()) {
                    if (pv->checkIsCloseEnough(childPv, 1.0f)) {
                        childPv->attach(pv);
                        visited.insert(make_pair(childPv, true));
                        polygonQueue.push(childPv);
                    }
                }
            }
        }
    }

    PolygonView* FoldPaperMakeLayer::getRootPolygonView() {
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

    int FoldPaperMakeLayer::getTreeNum(PolygonView* rootPolygon) {
        int count = 0;
        for (PolygonView* child : rootPolygon->childPolygonViewList) {
            count += getTreeNum(child);
        }
        return count + 1;
    }

    void FoldPaperMakeLayer::setPolygonsTouchEnabled(bool touchEnabled) {
        for (PolygonView* v : polygonViewList) {
            v->setTouchEnabled(touchEnabled);
        }
    }

    int FoldPaperMakeLayer::getMaxOrder() {
        Vector<Node*> nodeList = doContainerNode->getChildren();
        if (nodeList.size() == 0) return 0;
        Node* node = nodeList.at(doContainerNode->getChildrenCount() - 1); //获取最上面的那个Node
        return node->getLocalZOrder() + 1;
    }
}