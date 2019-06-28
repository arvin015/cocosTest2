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
    textureName(""),
    radius(0) {

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

        polygonDrawNode = DrawNodeEx::create();
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
        if (polygonType == ARC) { //圆弧，贴图位置下移
            textureSprite->setPosition(Vec2(polygonDrawNode->getContentSize() / 2) - Vec2(0, 10));
        } else if (polygonType == TRIANGLE) { //等腰三角形，贴图位置下移
            textureSprite->setPosition(Vec2(polygonDrawNode->getContentSize() / 2) - Vec2(0, 20));
        } else {
            textureSprite->setPosition(polygonDrawNode->getContentSize() / 2);
        }
        textureSprite->setScale(getTextureScale());
        polygonDrawNode->addChild(textureSprite);

        //顶点加上旋转精灵
        rotateSpriteList.clear();
        float rotateDegree = 180;
        float perDegree;
        if (polygonType == ARC) { //圆弧的旋转精灵特殊处理
            perDegree = 360.0 / 3;
            vector<Vec2> vertexList; //构造三个点
            vertexList.push_back(polygon.centerPoint + Vec2(0, radius / 2));
            vertexList.push_back(Vec2(0, radius - sinf(M_PI / 4) * radius));
            vertexList.push_back(Vec2(sinf(M_PI / 4) * radius * 2, radius - sinf(M_PI / 4) * radius));

            for (int i = 0; i < vertexList.size(); i++) {
                addRotateSprite(vertexList[i], rotateDegree);
                rotateDegree += perDegree;
            }
        } else {
            perDegree = 360.0 / polygon.getVertexNum();
            for (int i = 0; i < polygon.getVertexNum(); i++) {
                addRotateSprite(polygon.getVertexPos(i), rotateDegree);
                rotateDegree += perDegree;
            }
        }
    }

    void PolygonView::addRotateSprite(const Vec2 &pos, float rotateDegree) {
        auto sprite = Sprite::create("quadrangle_btn_line_rotate2.png");
        sprite->setPosition(pos);
        sprite->setScale(R_SCALE);
        sprite->setRotation(rotateDegree);
        sprite->setVisible(false);
        polygonDrawNode->addChild(sprite);
        rotateSpriteList.pushBack(sprite);
    }
    
    bool PolygonView::onTouchBegan(Touch* touch, Event* event) {
        if (!isTouchEnabled) {
            return false;
        }

        Vec2 location = getPolygonViewNodePoint(touch->getLocation());

        //未选中多边形
        if (!isSelected) {
            if (checkIsInPolygon(location)) { //移位
                setPolygonSelectedState(true);
                touchType = MOVE;
                touchListener->setSwallowTouches(true); //禁止穿透
                bringToFront(); //置顶
            }
        } else { //已经选中多边形
            if (checkIsRotatePolygon(location)) { //旋转
                touchType = RORATE;
            } else if (checkIsInPolygon(location)) { //移位
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
            float degree = CC_RADIANS_TO_DEGREES(d2.getAngle(d1));
            rotatePolygon(fmodf(getPolygonRotate() + degree, 360));
        } else if (touchType == MOVE) { //平移
            movePolygon(getPolygonPosition() + touch->getDelta());
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

    bool PolygonView::checkIsInPolygon(const Vec2 &location) {
        if (polygonType == ARC) { //圆弧特殊处理-满足两个条件：1. 圆心距离；2. 三角形内

            Vec2 cp = polygon.centerPoint + Vec2(0, radius / 2); //所在圆的圆心点
            if (cp.distance(location) <= radius) { //满足条件1
                vector<Vec2> vertexList; //构造三角形三个点
                vertexList.push_back(cp);
                vertexList.push_back(Vec2(sinf(M_PI / 4) * radius + radius, 0));
                vertexList.push_back(Vec2(sinf(M_PI / 4) * radius - radius, 0));

                int crossTotal = 0;
                for (int i = 0; i < 3; i++) {
                    Vec2 p1 = vertexList[i];
                    Vec2 p2 = vertexList[(i + 1) % 3];
                    Vec2 temp;
                    if (intersection(p1, p2, location, Vec2(-300, location.y), temp) > 0) {
                        crossTotal++;
                    }
                }
                return crossTotal % 2 == 1;
            }
            return false;
        } else {
            return polygon.checkIsInPolygon(location);
        }
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

        if (polygonType == CIRCLE) { //绘制圆
            polygonDrawNode->drawSolidCircleWithBorder(polygon.getCenterPoint(), radius, 120, fillColor, 1, Color4F::BLACK);
        } else if (polygonType == ARC) { //绘制圆弧
            float cx = polygon.getCenterPoint().x;
            float cy = polygon.getCenterPoint().y + polygon.edgeList[1].getLength() / 2;
            polygonDrawNode->drawSolidArc(Vec2(cx, cy), radius, 225, 90, 120, fillColor, 1, Color4F::BLACK);
        } else { //绘制多边形
            Vec2* vertexs = new Vec2[polygon.getVertexNum()];
            for (int i = 0; i < polygon.getVertexNum(); i++) {
                vertexs[i] = polygon.vertexList[i].position;
            }
            polygonDrawNode->drawPolygon(vertexs, polygon.getVertexNum(), fillColor, 1,
                                         Color4F::BLACK);
            delete [] vertexs;
        }
    }

    float PolygonView::getTextureScale() {
        float scale = fminf(polygonDrawNode->getContentSize().width, polygonDrawNode->getContentSize().height) / TEXTURE_SIZE;
        if (polygonType == ARC) {
            scale -= 0.1f;
        } else if (polygonType == TRIANGLE) {
            scale -= 0.13f;
        } else if (polygon.getVertexNum() == 3 || polygon.getVertexNum() == 5) {
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
        float angle_inc = -2 * M_PI / (float) edge;
        float angle = 0.5f * M_PI - 0.5f * angle_inc;
        for (int i = 0; i < edge; i++) {
            float x = radius * cos(angle);
            float y = radius * sin(angle);

            float r = edgeLength / radius;
            float u = (0.5f + r * 0.5f * cos(angle + M_PI));
            float v = (0.5f + r * 0.5f * sin(angle + M_PI));

            vertexList.push_back(Vertex2D(centerPoint + Vec2(x, y), Vec2(1.0f - u, v)));
            angle += angle_inc;
        }

        polygon = Polygon(vertexList, centerPoint);
    }

    void PolygonView::createRegularPolygonWithEdgeLength(const Vec2 &center, int edge, float edgeLength) {
        if (edge < 3)
            return;

        float angle = M_PI / (float) edge;
        float radius = edgeLength / (2.0f * sin(angle));
        createRegularPolygonWithRadius(center, edge, radius);
    }

    void PolygonView::createCircle(const cocos2d::Vec2 &centerPoint, float radius) {
        this->radius = radius;

        vector<Vertex2D> vertexList;

        float cx = centerPoint.x;
        float cy = centerPoint.y;

        //通过改变纹理坐标来保证图案不会变形
        float minX = 0.0f, minY = 0.0f, maxX = 1.0f, maxY = 1.0f;

        vertexList.push_back(Vertex2D(Vec2(cx - radius, cy + radius), Vec2(minX, minY)));
        vertexList.push_back(Vertex2D(Vec2(cx + radius, cy + radius), Vec2(maxX, minY)));
        vertexList.push_back(Vertex2D(Vec2(cx + radius, cy - radius), Vec2(maxX, maxY)));
        vertexList.push_back(Vertex2D(Vec2(cx - radius, cy - radius), Vec2(minX, maxY)));

        polygon = Polygon(vertexList, centerPoint);
    }

    void PolygonView::createArc(const cocos2d::Vec2 &centerPoint, float radius) {
        this->radius = radius;

        vector<Vertex2D> vertexList;

        float width = radius * sinf(M_PI / 4) * 2;
        float height = radius;

        float cx = centerPoint.x;
        float cy = centerPoint.y - height / 2;

        //通过改变纹理坐标来保证图案不会变形
        float minX = 0.0f, minY = 0.0f, maxX = 1.0f, maxY = 1.0f;

        vertexList.push_back(Vertex2D(Vec2(cx - width / 2, cy + height / 2), Vec2(minX, minY)));
        vertexList.push_back(Vertex2D(Vec2(cx + width / 2, cy + height / 2), Vec2(maxX, minY)));
        vertexList.push_back(Vertex2D(Vec2(cx + width / 2, cy - height / 2), Vec2(maxX, maxY)));
        vertexList.push_back(Vertex2D(Vec2(cx - width / 2, cy - height / 2), Vec2(minX, maxY)));

        polygon = Polygon(vertexList, Vec2(cx, cy));
    }

    void PolygonView::createTriangle(const Vec2 &centerPoint, float baseLength, float broadsideLength) {

        float h = sqrtf(broadsideLength * broadsideLength - baseLength * baseLength);

        vector<Vertex2D> vertexList;
        vertexList.push_back(Vertex2D(Vec2(centerPoint.x, centerPoint.y + h / 2), Vec2(0.5, -1.16)));
        vertexList.push_back(Vertex2D(Vec2(centerPoint.x + baseLength / 2, centerPoint.y - h / 2), Vec2(1.33, 1.2)));
        vertexList.push_back(Vertex2D(Vec2(centerPoint.x - baseLength / 2, centerPoint.y - h / 2), Vec2(-0.33, 1.2)));

        polygon = Polygon(vertexList, centerPoint);
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

    void PolygonView::movePolygon(const Vec2 &pos) {
        if (polygonDrawNode != nullptr) {
            polygonDrawNode->setPosition(pos);
        }
    }

    void PolygonView::rotatePolygon(float degree) {
        if (polygonDrawNode != nullptr) {
            polygonDrawNode->setRotation(degree);
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

        //圆形需要特殊处理---两种情况，1：圆碰长方形、2：长方形碰圆
        if ((polygonType == CIRCLE && otherPolygon->polygonType == RECTANGLE) ||
                (polygonType == RECTANGLE && otherPolygon->polygonType == CIRCLE)) {

            PolygonView* rp = polygonType == RECTANGLE ? this : otherPolygon;
            PolygonView* cp = polygonType == CIRCLE ? this : otherPolygon;

            Vec2 circleP = rp->getPolygonViewNodePoint(cp->getPolygonPosition()); //圆心点，相对于长方形坐标
            Vec2 cc; //最短距离落点
            if (rp->polygon.isCircleCloseEnough(circleP, minDistance, cp->radius, cc)) {
                if (needAttach) {
                    float dy = circleP.distance(cc) - cp->radius;
                    if (circleP.y > rp->polygon.getCenterPoint().y) {
                        dy = -dy;
                    }
                    if (polygonType == RECTANGLE) {
                        dy = -dy;
                    }
                    Vec2 p = (polygonType == RECTANGLE ? rp->polygon.getCenterPoint() : circleP) + Vec2(0, dy);
                    movePolygon(rp->getPolygonViewWorldPoint(p));
                }
                return true;
            }
            return false;
        } else {
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
                            rotatePolygon(getPolygonRotate() + differDegree);

                            //平移至与参照多边形贴合
                            Vec2 newMidPoint = getPolygonViewWorldPoint(edge.getMidPoint());
                            movePolygon(getPolygonPosition() + Vec2(midPoint1.x - newMidPoint.x, midPoint1.y - newMidPoint.y));
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool PolygonView::checkIsCloseEnough2(PolygonView* otherPolygon, float minDistance, bool needAttach) {
        PolygonView* circleP = otherPolygon->polygonType == CIRCLE ? otherPolygon : this;
        PolygonView* arcP = otherPolygon->polygonType == ARC ? otherPolygon : this;

        float cRadius = circleP->radius;
        float aRadius = arcP->radius;
        
        //以下坐标都是在圆弧坐标系下
        //需满足两个条件：1. 圆心距离<=minDistance；2. 矩形区域内
        Vec2 arcCenterP = arcP->polygon.getCenterPoint();
        Vec2 circleCenterP = arcP->getPolygonViewNodePoint(circleP->getPolygonPosition());
        Vec2 arcC = Vec2(arcCenterP.x, arcCenterP.y + aRadius / 2); //圆弧对应大圆的圆心点
        float dis = (aRadius + cRadius) - arcC.distance(circleCenterP);
        if (fabsf(dis) <= minDistance) { //满足条件1
            Rect rect;
            rect.origin = arcP->polygon.vertexList[3].position - Vec2(20, 60);
            rect.size = arcP->polygon.boundBox.size + Size(40, 60);

            if (rect.containsPoint(circleCenterP)) { //满足条件2
                if (needAttach) {
                    float ang = getAngleWith2Point(arcC, circleCenterP);
                    float dx = cosf(ang) * dis;
                    float dy = sinf(ang) * dis;
                    if (ang > 0) {
                        dx = -dx;
                        dy = -dy;
                    }
                    if (polygonType == ARC) { //圆弧去碰圆
                        dx = -dx;
                        dy = -dy;
                    }
                    Vec2 p = (polygonType == ARC ? arcCenterP : circleCenterP) + Vec2(dx, dy);
                    movePolygon(arcP->getPolygonViewWorldPoint(p));
                }
                return true;
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
