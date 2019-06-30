//
// Created by Administrator on 2019/6/1.
//

#include "PolyView.h"

USING_NS_CC;

PolyView::PolyView()
: touchListener(nullptr)
, drawNodeEx(nullptr)
, polyType(NONE)
, isSelect(false) {

}

PolyView::~PolyView() {

}

void PolyView::onEnter() {
    Node::onEnter();

    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
        if (checkIsContainerPoint(touch->getLocation())) {
            isSelect = true;
            bringToFront();
            touchListener->setSwallowTouches(true);
        }
        return true;
    };
    touchListener->onTouchMoved = [this](Touch* touch, Event* event) {
        if (isSelect) {
            setPosition(getPosition() + touch->getDelta());
        }
    };
    touchListener->onTouchEnded = [this](Touch* touch, Event* event) {
        if (isSelect) {
            if (touchEndCallback) {
                this->touchEndCallback();
            }
            isSelect = false;
            touchListener->setSwallowTouches(false);
        }
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void PolyView::onExit() {
    if (touchListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    }
    Node::onExit();
}

bool PolyView::init() {
    if (!Node::init()) {
        return false;
    }

    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx);

    return true;
}

void PolyView::createArc(const Vec2 &centerP, float radius) {
    this->polyType = ARC;
    this->radius = radius;

    float w = radius * sinf(CC_DEGREES_TO_RADIANS(45)) * 2;
    float h = radius;
    setContentSize(Size(w, h));
    setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    setPosition(Vec2(centerP.x, centerP.y - radius / 2));

    drawNodeEx->drawSolidArc(Vec2(w / 2, h), radius, 225, 90, 100, Color4F::YELLOW, 1, Color4F::BLUE);
}

void PolyView::createCircle(const Vec2 &centerP, float radius) {
    this->polyType = CIRCLE;
    this->radius = radius;

    float w = radius * 2;
    float h = w;
    setContentSize(Size(w, h));
    setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    setPosition(centerP);

    drawNodeEx->drawSolidCircleWithBorder(Vec2(w / 2, h / 2), radius, 100, Color4F::YELLOW, 1, Color4F::BLUE);
}

bool PolyView::checkIsCloseEnough(PolyView* otherPolyView) {
    Vec2 curCenterP = getCenterP();
    Vec2 otherCenterP = otherPolyView->getCenterP();
    float distance = fabsf(curCenterP.distance(otherCenterP) - (getRadius() + otherPolyView->getRadius()));
    if (distance < 30) { //距离满足条件
        Vec2 arcCP, arcBP, arcLP, arcRP, circleCP;
        if (getPolyType() == CIRCLE && otherPolyView->getPolyType() == ARC) {
            arcCP = otherCenterP;
            circleCP = curCenterP;
            arcBP = Vec2(arcCP.x, arcCP.y - otherPolyView->getContentSize().height);
            arcLP = (arcBP - arcCP).getNormalized().rotateByAngle(Vec2::ZERO, -CC_DEGREES_TO_RADIANS(45)) * otherPolyView->getRadius() + arcCP;
            arcRP = (arcBP - arcCP).getNormalized().rotateByAngle(Vec2::ZERO, CC_DEGREES_TO_RADIANS(45)) * otherPolyView->getRadius() + arcCP;
        } else if (getPolyType() == ARC && otherPolyView->getPolyType() == CIRCLE) {
            arcCP = curCenterP;
            circleCP = otherCenterP;
            arcBP = Vec2(arcCP.x, arcCP.y - getContentSize().height);
            arcLP = (arcBP - arcCP).getNormalized().rotateByAngle(Vec2::ZERO, -CC_DEGREES_TO_RADIANS(45)) * getRadius() + arcCP;
            arcRP = (arcBP - arcCP).getNormalized().rotateByAngle(Vec2::ZERO, CC_DEGREES_TO_RADIANS(45)) * getRadius() + arcCP;
        }

        Vec2 v1 = circleCP - arcCP;
        Vec2 v2 = arcLP - arcCP;
        bool lok = v2.cross(v1) > 0;

        Vec2 v3 = arcRP - arcCP;
        bool rok = v3.cross(v1) < 0;

        return lok && rok;
    }
    return false;
}

void PolyView::attach(PolyView* otherPolyView) {
    Vec2 curCenterP = getCenterP();
    Vec2 otherCenterP = otherPolyView->getCenterP();
    Vec2 normal = (curCenterP - otherCenterP).getNormalized();
    Vec2 targetV = normal * (getRadius() + otherPolyView->getRadius());
    Vec2 newCenterP = targetV + otherCenterP;
    if (getPolyType() == CIRCLE && otherPolyView->getPolyType() == ARC) {
        setPosition(newCenterP);
    } else if (getPolyType() == ARC && otherPolyView->getPolyType() == CIRCLE) {
        setPosition(Vec2(newCenterP.x, newCenterP.y - getContentSize().height / 2));
    }
}

Vec2 PolyView::getCenterP() {
    if (polyType == CIRCLE) {
        return getPosition();
    } else if (polyType == ARC) {
        return Vec2(getPosition().x, getPosition().y + getContentSize().height / 2);
    }
    return Vec2::ZERO;
}

bool PolyView::checkIsContainerPoint(const Vec2 &touchP) {
    if (polyType == CIRCLE) {
        return getBoundingBox().containsPoint(touchP);
    } else if (polyType == ARC) {
        if (getBoundingBox().containsPoint(touchP)) {
            Vec2 arcCP = getCenterP();
            Vec2 arcBP = Vec2(arcCP.x, arcCP.y - getContentSize().height);
            Vec2 arcLP = Vec2(arcBP.x - radius * tanf(CC_DEGREES_TO_RADIANS(45)), arcBP.y);
            Vec2 arcRP = Vec2(arcBP.x + radius * tanf(CC_DEGREES_TO_RADIANS(45)), arcBP.y);

            Vec2 v = touchP - arcCP;
            Vec2 v1 = arcLP - arcCP;
            Vec2 v2 = arcRP - arcCP;
            return v1.cross(v) >= 0 && v2.cross(v) <= 0;
        }
        return false;
    }
}

void PolyView::bringToFront() {
    Vector<Node*> nodeList = getParent()->getChildren();
    Node* node = nodeList.at(getParent()->getChildrenCount() - 1); //获取最上面的那个Node
    getParent()->reorderChild(this, node->getLocalZOrder() + 1);
}