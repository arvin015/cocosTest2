//
//  PolygonView.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#include "PolygonView.h"
#include "Polygon.h"
#include "MathUtils.h"
#include "BasicData.h"
#include "Slab.h"

USING_NS_CC;
using namespace std;

namespace FoldPaper {

#define PI 3.14
#define R_SCALE 0.6
#define MaxDegree 15
#define TEXTURE_SIZE 256.0f

    Slab makeSlab(PolygonView* polygonView, const Vec2& pt0, const Vec2& pt1) {
        Slab s;
        Vec2 v0 = pt1 - pt0;
        Vec2 n  = Vec2(-v0.y, v0.x);
        n.normalize();
        n = n.getNormalized();
        int size = polygonView->polygon.getVertexNum();
        for (int i = 0; i < size; i++) {
            Vec2 v = polygonView->getPolygonViewWorldPoint(polygonView->polygon.getVertexPos(i)) - pt0;
            float d = 0.0f;
            if (v.length() > 0.0f) {
                d = v.dot(n);
            }
            s.insert(d);
        }
        return s;
    }

    PolygonView::PolygonView() :
    touchListener(nullptr),
    polygonDrawNode(nullptr),
    fillColor(Color4F(76 / 255.0, 198 / 255.0, 198 / 255.0, 1)),
    isTouchEnabled(true),
    isSelected(false),
    isMove(false),
    touchType(NONE),
    touchEndCallback(nullptr),
    parentPolygonView(nullptr),
    faceType(FaceType::FaceTypeUnknown),
    textureSprite(nullptr),
    textureName("") {

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

        //加上贴图
        textureSprite = Sprite::create();
        textureSprite->setPosition(polygonDrawNode->getContentSize() / 2);
        textureSprite->setScale(getTextureScale());
        polygonDrawNode->addChild(textureSprite);

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
            vertexs[i] = polygon.vertexList[i].position;
        }
        polygonDrawNode->drawPolygon(vertexs, polygon.getVertexNum(), fillColor, 1,
                                     Color4F::BLACK);
        delete [] vertexs;
    }

    float PolygonView::getTextureScale() {
        float scale = fminf(polygonDrawNode->getContentSize().width, polygonDrawNode->getContentSize().height) / TEXTURE_SIZE;
        if (polygon.getVertexNum() == 3 || polygon.getVertexNum() == 5) {
            scale -= 0.1f;
        }
        return scale;
    }

    void PolygonView::createSquare(const Vec2 &centerPoint, float width, float height) {
        if (width <= 0.0f || height <= 0.0f)
            return;

        vector<Vertex2D> vertexList;

        float cx = centerPoint.x;
        float cy = centerPoint.y;

        //通过改变纹理坐标来保证图案不会变形
        float minX = 0.0f, minY = 0.0f, maxX = 1.0f, maxY = 1.0f; //正方形

        if (fabsf(width - height) > 0.1f) { //长方形
            if (width >= height) {
                float s = width / height;
                minX = 0.5f - 0.5f * s;
                maxX = 0.5f + 0.5f * s;
            } else {
                float s = height / width;
                minY = 0.5f - 0.5f * s;
                maxY = 0.5f + 0.5f * s;
            }
        }
        vertexList.push_back(Vertex2D(Vec2(cx - width / 2, cy + height / 2), Vec2(minX, minY)));
        vertexList.push_back(Vertex2D(Vec2(cx + width / 2, cy + height / 2), Vec2(maxX, minY)));
        vertexList.push_back(Vertex2D(Vec2(cx + width / 2, cy - height / 2), Vec2(maxX, maxY)));
        vertexList.push_back(Vertex2D(Vec2(cx - width / 2, cy - height / 2), Vec2(minX, maxY)));

        polygon = Polygon(vertexList, centerPoint);
    }

    void PolygonView::createRegularPolygonWithRadius(const Vec2 &centerPoint, int edge, float radius) {
        if (edge < 3)
            return;

        vector<Vertex2D> vertexList;

        float edgeLength = radius * 2.0f * sin(3.14 / (float)edge);
        float angle_inc = -2 * PI / (float) edge;
        float angle = 0.5f * PI - 0.5f * angle_inc;
        for (int i = 0; i < edge; i++) {
            float x = radius * cos(angle);
            float y = radius * sin(angle);

            float r = edgeLength / radius;
            float u = (0.5f + r * 0.5f * cos(angle + PI));
            float v = (0.5f + r * 0.5f * sin(angle + PI));

            vertexList.push_back(Vertex2D(centerPoint + Vec2(x, y), Vec2(1.0f - u, v)));
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

    void PolygonView::setTextureName(const string &textureName) {
        this->textureName = textureName;
        textureSprite->setTexture(textureName);
    }

    unsigned int PolygonView::getTextureId() {
        return textureName.empty() ? 0 : textureSprite->getTexture()->getName();
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

        for (int i = 0; i < polygon.edgeList.size(); i++) {
            Edge edge = polygon.edgeList.at(i);
            Vec2 preWorldPoint = getPolygonViewWorldPoint(edge.prePoint);
            Vec2 nextWorldPoint = getPolygonViewWorldPoint(edge.nextPoint);

            for (int j = 0; j < otherPolygon->polygon.edgeList.size(); j++) {
                Edge e = otherPolygon->polygon.edgeList.at(j);
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
        if (otherPolygon == nullptr) return false;

        int size0 = polygon.getVertexNum();
        for (int i = 0; i < size0; i++) {
            Vec2 pt0 = getPolygonViewWorldPoint(polygon.getVertexPos(i));
            Vec2 pt1 = getPolygonViewWorldPoint(polygon.getVertexPos((i + 1) % size0));

            Slab s0 = makeSlab(this, pt0, pt1);
            Slab s1 = makeSlab(otherPolygon, pt0, pt1);
            if (!s0.overlapped(s1)) return false;
        }

        int size1 = otherPolygon->polygon.getVertexNum();
        for (int i = 0; i < size1; i++) {
            Vec2 pt0 = otherPolygon->getPolygonViewWorldPoint(otherPolygon->polygon.getVertexPos(i));
            Vec2 pt1 = otherPolygon->getPolygonViewWorldPoint(otherPolygon->polygon.getVertexPos((i + 1) % size1));
            Slab s0 = makeSlab(this, pt0, pt1);
            Slab s1 = makeSlab(otherPolygon, pt0, pt1);
            if (!s0.overlapped(s1)) return false;
        }
        return true;
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
