//
// Created by mac_arvin on 2018/11/8.
//

#include "FoldPaperMakeLayer.h"
#include "BasicData.h"

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

    void FoldPaperMakeLayer::createPolygonViewFromPrism(int side, float edgeLength) {
        if (side < 3) return;

        PolygonType type = P_UNKNOWN;
        switch (side) {
            case 3:
                type = TRIANGLE;
                break;
            case 4:
                type = RECTANGLE;
                break;
            case 5:
                type = FIVE_POLY;
                break;
            case 6:
                type = SIX_POLY;
                break;
        }

        float beginX = 100.0f;
        float beginY = 100.0f;

        for (int i = 0; i < side + 2; i++) {
            if (i == 0 || i == 1) { //上下面
                createPolygonView(type, Vec2(beginX + i * 100, beginY + i * 80), side, edgeLength, edgeLength, FaceTypeBase);
            } else { //侧面
                createPolygonView(RECTANGLE, Vec2(beginX + i * 100, beginY + i * 80), 4, edgeLength, edgeLength * PHI, FaceTypeLateralFace);
            }

        }
    }

    void FoldPaperMakeLayer::createPolygonViewFromCube(float edgeLength) {
        float beginX = 100.0f;
        float beginY = 100.0f;

        for (int i = 0; i < 6; i++) {
            if (i == 0 || i == 1) { //上下面
                createPolygonView(SQUARE, Vec2(beginX + i * 100, beginY + i * 80), 4, edgeLength, edgeLength, FaceTypeBase);
            } else { //侧面
                createPolygonView(SQUARE, Vec2(beginX + i * 100, beginY + i * 80), 4, edgeLength, edgeLength, FaceTypeLateralFace);
            }
        }
    }

    void FoldPaperMakeLayer::createPolygonView(int polygonType, const Vec2 &centerPoint, int edge, float edgeLength, float height, int faceType) {
        PolygonView* polygonView = PolygonView::create();
        switch ((PolygonType)polygonType) {
            case SQUARE:
            case RECTANGLE:
                polygonView->createSquare(centerPoint, edgeLength, height);
                break;
            case TRIANGLE:
            case FIVE_POLY:
            case SIX_POLY:
                polygonView->createRegularPolygonWithEdgeLength(centerPoint, edge, edgeLength);
                break;
        }
        polygonView->setTag(ids);
        polygonView->faceType = faceType;
        polygonView->initView();
        polygonView->setOnTouchEndCallback([this, polygonView]() {
            attachPolygons(polygonView);
        });
        polygonView->setOnSelectCallback([this, polygonView]() {
            selectedPolygonView = polygonView;
        });
        doContainerNode->addChild(polygonView, getMaxOrder());

        polygonViewList.pushBack(polygonView);

        if (faceType == FaceTypeUnknown) {
            if (selectedPolygonView != nullptr) {
                selectedPolygonView->setPolygonSelectedState(false);
            }
            selectedPolygonView = polygonView;
            selectedPolygonView->setPolygonSelectedState(true);
        }

        ids++;
    }

    void FoldPaperMakeLayer::responseColorClick(const cocos2d::Color4F &color) {
        if (selectedPolygonView != nullptr) {
            selectedPolygonView->updatePolygonColor(color);
        }
    }

    void FoldPaperMakeLayer::responseTextureClick(const string &textureName) {
        if (selectedPolygonView != nullptr) {
            selectedPolygonView->setTextureName(textureName);
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

    void FoldPaperMakeLayer::responseResetClick() {
        doContainerNode->removeAllChildren();
        polygonViewList.clear();
        rootPolygonView = nullptr;
        selectedPolygonView = nullptr;
        ids = 1;
        isTouchEnabled = true;
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

        graph.clear();
        visited.clear();
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