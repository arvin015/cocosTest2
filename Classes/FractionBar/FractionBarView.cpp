//
//  FractionBarView.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/12/26.
//

#include "FractionBarView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

namespace FractionBar {

    //是否碰撞，不包含边界
    bool intersectsRect(const Rect& rect1, const Rect& rect2) {
        return !(rect1.getMaxX() <= rect2.getMinX() ||
                 rect2.getMaxX() <= rect1.getMinX() ||
                 rect1.getMaxY() <= rect2.getMinY() ||
                 rect2.getMaxY() <= rect1.getMinY());
    }

    const int TEXT_SIZE = 18;
    const float ATTACH_DIS = 20.0f;

    FractionBarView::FractionBarView()
    : block(nullptr)
    , leftText(nullptr)
    , topText(nullptr)
    , bottomText(nullptr)
    , divideLine(nullptr)
    , lockSprite(nullptr)
    , borderDraw(nullptr)
    , isTouchEnabled(true)
    , isSelected(false)
    , isLockSelected(false)
    , isLocked(false) {

    }

    FractionBarView::~FractionBarView() {

    }

    void FractionBarView::onEnter() {
        Node::onEnter();

        touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = CC_CALLBACK_2(FractionBarView::onTouchBegan, this);
        touchListener->onTouchMoved = CC_CALLBACK_2(FractionBarView::onTouchMoved, this);
        touchListener->onTouchEnded = CC_CALLBACK_2(FractionBarView::onTouchEnded, this);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    }

    void FractionBarView::onExit() {
        if (touchListener != nullptr) {
            Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
        }

        Node::onExit();
    }

    FractionBarView* FractionBarView::create(const Size &size, const Vec2 &position, const Color3B &color, int left, int top, int bottom) {
        FractionBarView *node = new(std::nothrow)FractionBarView();
        if (node && node->init(size, position, color, left, top, bottom)) {
            node->autorelease();
            return node;
        }
        CC_SAFE_DELETE(node);
        return nullptr;
    }

    bool FractionBarView::init(const Size &size, const Vec2 &position, const Color3B &color, int left, int top, int bottom) {
        if (!Node::init()) {
            return false;
        }

        setContentSize(size);
        setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        setPosition(position);

        //颜色块
        block = Layout::create();
        block->setContentSize(size);
        block->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        block->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
        block->setBackGroundColor(color);
        block->setPosition(Vec2(size / 2));
        addChild(block);

        { //分数
            if (left != 0 && top == 0 && bottom == 0) { //整数
                leftText = Text::create();
                leftText->setPosition(Vec2(size / 2));
                leftText->setTextColor(Color4B::WHITE);
                leftText->setFontSize(TEXT_SIZE);
                leftText->setString(Value(left).asString());
                addChild(leftText);
            } else if (top != 0 && bottom != 0) { //分数

                divideLine = Layout::create();
                divideLine->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                divideLine->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
                divideLine->setBackGroundColor(Color3B::WHITE);
                addChild(divideLine);

                topText = Text::create();
                topText->setString(Value(top).asString());
                topText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
                topText->setTextColor(Color4B::WHITE);
                topText->setFontSize(TEXT_SIZE);
                divideLine->addChild(topText);

                bottomText = Text::create();
                bottomText->setString(Value(bottom).asString());
                bottomText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
                bottomText->setTextColor(Color4B::WHITE);
                bottomText->setFontSize(TEXT_SIZE);
                divideLine->addChild(bottomText);

                if (left != 0) { //假分数
                    leftText = Text::create();
                    leftText->setString(Value(left).asString());
                    leftText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
                    leftText->setTextColor(Color4B::WHITE);
                    leftText->setFontSize(TEXT_SIZE);
                    divideLine->addChild(leftText);
                }

                divideLine->setContentSize(Size(MAX(topText->getContentSize().width, bottomText->getContentSize().width) + 18, 1));
                divideLine->setPosition(Vec2(size / 2));
                if (leftText != nullptr) {
                    divideLine->setPositionX(divideLine->getPositionX() + leftText->getContentSize().width / 2);
                }
                topText->setPosition(Vec2(divideLine->getContentSize().width / 2, 1));
                bottomText->setPosition(Vec2(divideLine->getContentSize().width / 2, 0));
            }
        }

        //边框
        borderDraw = DrawNode::create();
        addChild(borderDraw);
        drawBorder(false);

        //锁
        lockSprite = Sprite::create("quadrangle_btn_line_unlock.png");
        lockSprite->setContentSize(Size(60, 60));
        lockSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        lockSprite->setPosition(Vec2(size.width / 2, size.height + 20));
        lockSprite->setVisible(false);
        lockSprite->setOpacity(0); //隐藏
        addChild(lockSprite);

        return true;
    }

    bool FractionBarView::onTouchBegan(Touch* touch, Event* event) {
        if (!isTouchEnabled || !isVisible()) return false;

        if (touch->getID() == 0) {
            Vec2 location = touch->getLocation();
            if (getBoundingBox().containsPoint(location)) {
                isSelected = true;
            } else if (lockSprite->getBoundingBox().containsPoint(convertToNodeSpace(location))) { //选中了锁
                lockSprite->setTexture(isLocked ? "quadrangle_btn_line_lock_mo.png" : "quadrangle_btn_line_unlock_mo.png");
                isLockSelected = true;
                isSelected = true;
            }

            if (isSelected) {
                bringToFront();
                touchListener->setSwallowTouches(true);

                if (selectCallback != nullptr) {
                    this->selectCallback();
                }
            }
            setBarSelected(isSelected);

            if (touchCallback != nullptr) {
                Vec2 p = Director::getInstance()->convertToUI(touch->getLocation());
                this->touchCallback(barId, 0, p.x, p.y);
            }
        }

        return true;
    }

    void FractionBarView::onTouchMoved(Touch* touch, cocos2d::Event* event) {
        if (touch->getID() == 0) {
            if (isSelected) {
                isMoved = true;
                if (!isLocked) setPosition(getPosition() + touch->getDelta());
            }

            if (touchCallback != nullptr) {
                Vec2 p = Director::getInstance()->convertToUI(touch->getLocation());
                this->touchCallback(barId, 1, p.x, p.y);
            }
        }
    }

    void FractionBarView::onTouchEnded(Touch* touch, Event* event) {
        if (touch->getID() == 0) {
            if (isSelected) {
                if (isLockSelected) {
                    if (!isMoved) {
                       isLocked = !isLocked;
                    } else {
                        if (!isLocked) isLocked = true;
                    }
                    lockSprite->setTexture(isLocked ? "quadrangle_btn_line_lock.png" : "quadrangle_btn_line_unlock.png");
                }
            }
            
            touchListener->setSwallowTouches(false);
            isMoved = false;
            isSelected = false;
            isLockSelected = false;

            if (touchCallback != nullptr) {
                Vec2 p = Director::getInstance()->convertToUI(touch->getLocation());
                this->touchCallback(barId, 2, p.x, p.y);
            }
        }
    }

    void FractionBarView::drawBorder(bool isSelected) {
        borderDraw->clear();
        borderDraw->setLineWidth(isSelected ? 2 : 1);
        Vec2 points[] = {
            Vec2::ZERO,
            Vec2(getContentSize().width, 0),
            Vec2(getContentSize()),
            Vec2(0, getContentSize().height)
        };
        borderDraw->drawPoly(points, 4, true, isSelected ? Color4F::BLUE : Color4F::BLACK);
    }

    void FractionBarView::setBarSelected(bool isSelected) {
        lockSprite->setVisible(isSelected);
        drawBorder(isSelected);
    }

    bool FractionBarView::isIntersect(FractionBarView* other) {
        return intersectsRect(getBoundingBox(), other->getBoundingBox());
    }

    bool FractionBarView::isCloseEnough(FractionBarView* other, float &dx, float &dy, bool &isCrash) {
        Rect rect1 = getBoundingBox();
        Rect rect2 = other->getBoundingBox();
        
        //四个偏方位排除
        if ((rect1.getMaxX() <= rect2.getMinX() && rect1.getMinY() >= rect2.getMaxY()) ||
            (rect1.getMinX() >= rect2.getMaxX() && rect1.getMinY() >= rect2.getMaxY()) ||
            (rect1.getMaxX() <= rect2.getMinX() && rect1.getMaxY() <= rect2.getMinY()) ||
            (rect1.getMinX() >= rect2.getMaxX() && rect1.getMaxY() <= rect2.getMinY())) {
            return false;
        }

        //四个正方位
        bool isOK = false;
        if (rect1.getMinY() >= rect2.getMaxY() && fabsf(rect1.getMinY() - rect2.getMaxY()) < ATTACH_DIS) {
            dy = rect2.getMaxY() - rect1.getMinY();
            if (fabsf(rect1.getMinX() - rect2.getMinX()) < ATTACH_DIS) {
                dx = rect2.getMinX() - rect1.getMinX();
            } else if (fabsf(rect1.getMaxX() - rect2.getMaxX()) < ATTACH_DIS) {
                dx = rect2.getMaxX() - rect1.getMaxX();
            }
            if (fabsf((rect1.getMinX() + rect1.size.width / 2) - (rect2.getMinX() + rect2.size.width / 2)) < ATTACH_DIS) {
                dx = (rect2.getMinX() + rect2.size.width / 2) - (rect1.getMinX() + rect1.size.width / 2);
            }
            isOK = true;
        } else if (rect1.getMinX() >= rect2.getMaxX() && fabsf(rect1.getMinX() - rect2.getMaxX()) < ATTACH_DIS) {
            dx = rect2.getMaxX() - rect1.getMinX();
            if (fabsf(rect1.getMinY() - rect2.getMinY()) < ATTACH_DIS) {
                dy = rect2.getMinY() - rect1.getMinY();
            } else if (fabsf(rect1.getMaxY() - rect2.getMaxY()) < ATTACH_DIS) {
                dy = rect2.getMaxY() - rect1.getMaxY();
            }
            isOK = true;
        } else if (rect1.getMaxY() <= rect2.getMinY() && fabsf(rect1.getMaxY() - rect2.getMinY()) < ATTACH_DIS) {
            dy = rect2.getMinY() - rect1.getMaxY();
            if (fabsf(rect1.getMinX() - rect2.getMinX()) < ATTACH_DIS) {
                dx = rect2.getMinX() - rect1.getMinX();
            } else if (fabsf(rect1.getMaxX() - rect2.getMaxX()) < ATTACH_DIS) {
                dx = rect2.getMaxX() - rect1.getMaxX();
            }
            if (fabsf((rect1.getMinX() + rect1.size.width / 2) - (rect2.getMinX() + rect2.size.width / 2)) < ATTACH_DIS) {
                dx = (rect2.getMinX() + rect2.size.width / 2) - (rect1.getMinX() + rect1.size.width / 2);
            }
            isOK = true;
        } else if (rect1.getMaxX() <= rect2.getMinX() && fabsf(rect1.getMaxX() - rect2.getMinX()) < ATTACH_DIS) {
            dx = rect2.getMinX() - rect1.getMaxX();
            if (fabsf(rect1.getMinY() - rect2.getMinY()) < ATTACH_DIS) {
                dy = rect2.getMinY() - rect1.getMinY();
            } else if (fabsf(rect1.getMaxY() - rect2.getMaxY()) < ATTACH_DIS) {
                dy = rect2.getMaxY() - rect1.getMaxY();
            }
            isOK = true;
        }
        if (isOK) {
            rect1.setRect(rect1.getMinX() + dx, rect1.getMinY() + dy, rect1.size.width, rect1.size.height);
            if (rect1.intersectsRect(rect2)) return true;
        } else {
            //重叠的
            bool isOkX = false;
            bool isOkY = false;
            if (fabsf(rect1.getMinX() - rect2.getMinX()) < ATTACH_DIS) {
                dx = rect2.getMinX() - rect1.getMinX();
                isOkX = true;
            } else if (fabsf(rect1.getMinX() - rect2.getMaxX()) < ATTACH_DIS) {
                dx = rect2.getMaxX() - rect1.getMinX();
                isOkX = true;
            } else if (fabsf(rect1.getMaxX() - rect2.getMaxX()) < ATTACH_DIS) {
                dx = rect2.getMaxX() - rect1.getMaxX();
                isOkX = true;
            } else if (fabsf(rect1.getMaxX() - rect2.getMinX()) < ATTACH_DIS) {
                dx = rect2.getMinX() - rect1.getMaxX();
                isOkX = true;
            }
            
            if (fabsf(rect1.getMinY() - rect2.getMinY()) < ATTACH_DIS) {
                dy = rect2.getMinY() - rect1.getMinY();
                isOkY = true;
            } else if (fabsf(rect1.getMinY() - rect2.getMaxY()) < ATTACH_DIS) {
                dy = rect2.getMaxY() - rect1.getMinY();
                isOkY = true;
            } else if (fabsf(rect1.getMaxY() - rect2.getMaxY()) < ATTACH_DIS) {
                dy = rect2.getMaxY() - rect1.getMaxY();
                isOkY = true;
            } else if (fabsf(rect1.getMaxY() - rect2.getMinY()) < ATTACH_DIS) {
                dy = rect2.getMinY() - rect1.getMaxY();
                isOkY = true;
            }
            
            //中间对齐
            if (isOkY) {
                if (fabsf((rect1.getMinX() + rect1.size.width / 2) - (rect2.getMinX() + rect2.size.width / 2)) < ATTACH_DIS) {
                    dx = (rect2.getMinX() + rect2.size.width / 2) - (rect1.getMinX() + rect1.size.width / 2);
                    isOkX = true;
                }
            }
            
            //吸附后是否碰撞
            rect1.setRect(rect1.getMinX() + dx, rect1.getMinY() + dy, rect1.size.width, rect1.size.height);
            isCrash = intersectsRect(rect1, rect2);
            
            if (isOkX || isOkY) {
                return rect1.intersectsRect(rect2);
            }
        }
        return false;
    }
    
    void FractionBarView::bringToFront() {
        Vector<Node*> nodeList = getParent()->getChildren();
        Node* node = nodeList.at(getParent()->getChildrenCount() - 1); //获取最上面的那个Node
        this->getParent()->reorderChild(this, node->getLocalZOrder() + 1);
    }
}
