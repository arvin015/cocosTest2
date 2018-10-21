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
#define R_SCALE 0.3

    PolygonView::PolygonView() :
    touchListener(nullptr),
    polygonDrawNode(nullptr),
    fillColor(Color4F(76 / 255.0, 198 / 255.0, 198 / 255.0, 1)),
    isTouchEnabled(true),
    isSelected(false),
    touchType(NONE),
    touchEndCallback(nullptr) {
        
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
            auto sprite = Sprite::create("icon_rotate.png");
            sprite->setPosition(polygon.getVertexPos(i));
            sprite->setScale(R_SCALE);
            sprite->setRotation(rotateDegree);
            sprite->setColor(Color3B::BLACK);
            sprite->setVisible(false);
            polygonDrawNode->addChild(sprite);
            rotateSpriteList.pushBack(sprite);

            rotateDegree += perDegree;
        }
    }
    
    bool PolygonView::onTouchBegan(Touch* touch, Event* event) {
        if (!isTouchEnabled) {
            return false;
        }
        
        Vec2 location = polygonDrawNode->convertToNodeSpace(touch->getLocation());

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
        if (touchType == RORATE) { //旋转
            Vec2 prePoint = touch->getPreviousLocation();
            Vec2 location = touch->getLocation();
            
            Vec2 dir0 = (prePoint - polygonDrawNode->getPosition()).getNormalized();
            Vec2 dir1 = (location - polygonDrawNode->getPosition()).getNormalized();
            
            float dot = dir0.dot(dir1);
            
            dot = dot < -1.0f ? -1.0f : dot;
            dot = dot > 1.0f ? 1.0f : dot;
            
            float deltaAngle = acos(dot);
            if (dir0.cross(dir1) < 0.0f)
                deltaAngle = -deltaAngle;
            
            float degree = CC_RADIANS_TO_DEGREES(deltaAngle);
            rotatePolygon(-degree);
        } else if (touchType == MOVE) { //平移
            movePolygon(touch->getDelta());
        }
    }
    
    void PolygonView::onTouchEnded(Touch* touch, Event* event) {
        if (touchType != NONE) { //检测吸附
            if (touchEndCallback != nullptr) {
                this->touchEndCallback();
            }
        }
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
            polygonDrawNode->setRotation(fmodf(polygonDrawNode->getRotation(), 360) + deltaDegree);
        }
    }

    void PolygonView::setPolygonSelectedState(bool isSelected) {
        this->isSelected = isSelected;

        //设置缩放精灵显示隐藏
        for (Sprite* rotateSprite : rotateSpriteList) {
            rotateSprite->setVisible(isSelected);
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
