//
//  PolygonView.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#include "PolygonView.h"
#include "Polygon.h"
#include "MathUtils.h"

USING_NS_CC;
using namespace std;

namespace FoldPaper {

#define PI 3.14
#define R_SCALE 0.6
#define MaxDegree 15

    PolygonView::PolygonView() :
    touchListener(nullptr),
    polygonDrawNode(nullptr),
    fillColor(Color4F(76 / 255.0, 198 / 255.0, 198 / 255.0, 1)),
    isTouchEnabled(true),
    isSelected(false),
    isMove(false),
    touchType(NONE),
    touchEndCallback(nullptr),
    parentPolygonView(nullptr) {
        
    }

    PolygonView::~PolygonView() {

    }

    void PolygonView::onEnter() {
        Node::onEnter();
        
        touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = CC_CALLBACK_2(PolygonView::onTouchBegan, this);
        touchListener->onTouchMoved = CC_CALLBACK_2(PolygonView::onTouchMoved, this);
        touchListener->onTouchEnded = CC_CALLBACK_2(PolygonView::onTouchEnded, this);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    }

    void PolygonView::onExit() {
        if (touchListener != nullptr) {
            Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
        }
        Node::onExit();
    }

    bool PolygonView::init() {
        if (!Node::init()) {
            return false;
        }

        setContentSize(Size(1024, 768));

        polygonDrawNode = DrawNode::create();
        polygonDrawNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        addChild(polygonDrawNode);

        return true;
    }

    void PolygonView::initView() {

        polygonDrawNode->setPosition(polygon.centerPoint);
        polygonDrawNode->setContentSize(polygon.boundBox.size);

        polygon.changePointsToNode(polygonDrawNode); //将多边形顶点坐标转成Node坐标

        //绘制
        onDraw();

        //顶点加上旋转精灵
        rotateSpriteList.clear();
        float perDegree = 360.0 / polygon.getVertexNum();
        float rotateDegree = 180;
        for (int i = 0; i < polygon.getVertexNum(); i++) {
            auto sprite = Sprite::create("quadrangle_btn_line_rotate2.png");
            sprite->setPosition(polygon.getVertexPos(i));
            sprite->setScale(R_SCALE);
            sprite->setRotation(rotateDegree);
            sprite->setVisible(false);
            polygonDrawNode->addChild(sprite);
            rotateSpriteList.pushBack(sprite);

            rotateDegree += perDegree;
        }

        setPolygonSelectedState(true);
    }
    
    bool PolygonView::onTouchBegan(Touch* touch, Event* event) {
        if (!isTouchEnabled) {
            return false;
        }

        Vec2 location = getPolygonViewNodePoint(touch->getLocation());

        //未选中多边形
        if (!isSelected) {
            if (polygon.checkIsInPolygon(location)) { //移位
                setPolygonSelectedState(true);
                touchType = MOVE;
                touchListener->setSwallowTouches(true); //禁止穿透
                bringToFront(); //置顶
            }
        } else { //已经选中多边形
            if (checkIsRotatePolygon(location)) { //旋转
                touchType = RORATE;
            } else if (polygon.checkIsInPolygon(location)) { //移位
                touchType = MOVE;
            } else {
                setPolygonSelectedState(false);
            }
            
            if (touchType == RORATE || touchType == MOVE) {
                touchListener->setSwallowTouches(true); //禁止穿透
            }
        }
        
        return true;
    }
    
    void PolygonView::onTouchMoved(Touch* touch, Event* event) {

        isMove = true;

        if (touchType == RORATE) { //旋转
            Vec2 prePoint = touch->getPreviousLocation();
            Vec2 location = touch->getLocation();

            Vec2 d1 = prePoint - polygonDrawNode->getPosition();
            Vec2 d2 = location - polygonDrawNode->getPosition();
            float deltaAngle = atan2f(d2.x, d2.y) - atan2f(d1.x, d1.y);
            float degree = CC_RADIANS_TO_DEGREES(deltaAngle);
            rotatePolygon(degree);
        } else if (touchType == MOVE) { //平移
            movePolygon(touch->getDelta());
        }
    }
    
    void PolygonView::onTouchEnded(Touch* touch, Event* event) {
        if (touchType != NONE && isMove) { //检测吸附
            if (touchEndCallback != nullptr) {
                this->touchEndCallback();
            }
        }
        isMove = false;
        touchType = NONE;
        touchListener->setSwallowTouches(false);
    }

    bool PolygonView::checkIsRotatePolygon(const cocos2d::Vec2 &pos) {
        for (Sprite* sprite : rotateSpriteList) {
            if (sprite->getBoundingBox().containsPoint(pos)) {
                return true;
            }
        }
        return false;
    }

    void PolygonView::onDraw() {

        polygonDrawNode->clear();

        //绘制多边形
        Vec2* vertexs = new Vec2[polygon.getVertexNum()];
        for (int i = 0; i < polygon.getVertexNum(); i++) {
            vertexs[i] = polygon.vertexList[i];
        }
        polygonDrawNode->drawPolygon(vertexs, polygon.getVertexNum(), fillColor, 1,
                                     Color4F::BLACK);
        delete [] vertexs;
    }

    void PolygonView::createSquare(const Vec2 &centerPoint, float width, float height) {
        if (width <= 0.0f || height <= 0.0f)
            return;

        vector<Vec2> vertexList;

        float cx = centerPoint.x;
        float cy = centerPoint.y;

        vertexList.push_back(Vec2(cx - width / 2, cy + height / 2));
        vertexList.push_back(Vec2(cx + width / 2, cy + height / 2));
        vertexList.push_back(Vec2(cx + width / 2, cy - height / 2));
        vertexList.push_back(Vec2(cx - width / 2, cy - height / 2));

        polygon = Polygon(vertexList, centerPoint);
    }

    void PolygonView::createRegularPolygonWithRadius(const Vec2 &centerPoint, int edge, float radius) {
        if (edge < 3)
            return;

        vector<Vec2> vertexList;

        float angle_inc = -2 * PI / (float) edge;
        float angle = 0.5f * PI - 0.5f * angle_inc;
        for (int i = 0; i < edge; i++) {
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            vertexList.push_back(centerPoint + Vec2(x, y));
            angle += angle_inc;
        }

        polygon = Polygon(vertexList, centerPoint);
    }

    void PolygonView::createRegularPolygonWithEdgeLength(const Vec2 &center, int edge, float edgeLength) {
        if (edge < 3)
            return;

        float angle = PI / (float) edge;
        float radius = edgeLength / (2.0f * sin(angle));
        createRegularPolygonWithRadius(center, edge, radius);
    }

    void PolygonView::updatePolygonColor(const Color4F &fillColor) {
        this->fillColor = fillColor;
        onDraw();
    }

    void PolygonView::movePolygon(const Vec2 &deltaPos) {
        if (polygonDrawNode != nullptr) {
            polygonDrawNode->setPosition(polygonDrawNode->getPosition() + deltaPos);
        }
    }

    void PolygonView::rotatePolygon(float deltaDegree) {
        if (polygonDrawNode != nullptr) {
            polygonDrawNode->setRotation(fmodf(polygonDrawNode->getRotation() + deltaDegree, 360));
        }
    }

    void PolygonView::addChildPolygonView(PolygonView* polygonView) {
        childPolygonViewList.pushBack(polygonView);
    }

    void PolygonView::removeAllChildPolygon() {
        childPolygonViewList.clear();
    }

    void PolygonView::attach(PolygonView* parent) {
        setParentPolygonView(parent);
        parent->addChildPolygonView(this);
    }

    void PolygonView::detach() {
        setParentPolygonView(nullptr);
        removeAllChildPolygon();
    }

    bool PolygonView::checkIsCloseEnough(PolygonView* otherPolygon, float minDistance, bool needAttach) {

        for (Edge edge : polygon.edgeList) {
            Vec2 preWorldPoint = getPolygonViewWorldPoint(edge.prePoint);
            Vec2 nextWorldPoint = getPolygonViewWorldPoint(edge.nextPoint);

            for (Edge e : otherPolygon->polygon.edgeList) {
                Vec2 point1 = otherPolygon->getPolygonViewWorldPoint(e.prePoint);
                Vec2 point2 = otherPolygon->getPolygonViewWorldPoint(e.nextPoint);
                Vec2 midPoint1 = otherPolygon->getPolygonViewWorldPoint(e.getMidPoint());

                if (Edge::isCloseEnough(point1, point2, preWorldPoint, nextWorldPoint, MaxDegree, minDistance)) { //满足吸附条件
                    if (needAttach) {
                        //旋转至与参照多边形相同角度
                        float differDegree = Edge::getDifferDegree(point1, point2, preWorldPoint, nextWorldPoint);
                        rotatePolygon(differDegree);

                        //平移至与参照多边形贴合
                        Vec2 newMidPoint = getPolygonViewWorldPoint(edge.getMidPoint());
                        movePolygon(Vec2(midPoint1.x - newMidPoint.x, midPoint1.y - newMidPoint.y));
                    }
                    return true;
                }
            }
        }

        return false;
    }

    bool PolygonView::checkIsOverlap(PolygonView* otherPolygon) {
        int count = 0;
        for (Edge edge : polygon.edgeList) {
            Vec2 preWorldPoint = getPolygonViewWorldPoint(edge.prePoint);
            Vec2 nextWorldPoint = getPolygonViewWorldPoint(edge.nextPoint);

            for (Edge e : otherPolygon->polygon.edgeList) {
                Vec2 point1 = otherPolygon->getPolygonViewWorldPoint(e.prePoint);
                Vec2 point2 = otherPolygon->getPolygonViewWorldPoint(e.nextPoint);

                if (Edge::isCloseEnough(point1, point2, preWorldPoint, nextWorldPoint, 1.0f, 1.0f)) {
                    count++;
                }
            }
        }

        return count > 1;
    }

    void PolygonView::setPolygonSelectedState(bool isSelected) {
        this->isSelected = isSelected;

        //设置缩放精灵显示隐藏
        for (Sprite* rotateSprite : rotateSpriteList) {
            rotateSprite->setVisible(isSelected);
        }

        if (isSelected) {
            if (selectCallback != nullptr) {
                this->selectCallback();
            }
        }
    }

    Vec2 PolygonView::getPolygonViewWorldPoint(const Vec2 &nodePoint) {
        return polygonDrawNode->convertToWorldSpace(nodePoint);
    }

    Vec2 PolygonView::getPolygonViewNodePoint(const Vec2 &worldPoint) {
        return polygonDrawNode->convertToNodeSpace(worldPoint);
    }
    
    void PolygonView::bringToFront() {
        Vector<Node*> nodeList = getParent()->getChildren();
        Node* node = nodeList.at(getParent()->getChildrenCount() - 1); //获取最上面的那个Node
        this->getParent()->reorderChild(this, node->getLocalZOrder() + 1);
    }
}
