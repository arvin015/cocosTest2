//
//  FractionBarLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/12/26.
//

#include "FractionBarLayer.h"
#include "FractionBarView.h"

USING_NS_CC;
using namespace std;

namespace FractionBar {

    const Color3B BarColors[] = {
        Color3B(0, 51, 153),
        Color3B(0, 0, 0),
        Color3B(152, 51, 0),
        Color3B(254, 221, 0),
        Color3B(50, 153, 51),
        Color3B(255, 153, 51),
        Color3B(255, 51, 51),
        Color3B(153, 51, 221),
        Color3B(255, 51, 153)
    };

    const float BarWidth = 600;
    const float BarHeight = 40;
    const float DefaultPosY = 768 / 2 + 100;

    FractionBarLayer::FractionBarLayer()
    : curFractionBarView(nullptr)
    , borderDraw(nullptr)
    , lockSprite(nullptr)
    , ids(1)
    , touchListener(nullptr) {

    }

    FractionBarLayer::~FractionBarLayer() {

    }

    void FractionBarLayer::onEnter() {
        BaseLayer::onEnter();

        touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
            curFractionBarView = nullptr;
            setLockSpriteVisible(false);
            return false;
        };
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    }

    void FractionBarLayer::onExit() {
        if (touchListener != nullptr) {
            Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
        }

        BaseLayer::onExit();
    }

    bool FractionBarLayer::init() {
        if (!BaseLayer::init()) {
            return false;
        }

        doContainer = Node::create();
        doContainer->setContentSize(Size(V_WIDTH, V_HEIGHT));
        addChild(doContainer);

        createFractionBar(1);
        createFractionBar(2);
        createFractionBar(3);

        borderDraw = DrawNode::create(10);
        addChild(borderDraw);

        lockSprite = Sprite::create();
        lockSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        lockSprite->setVisible(false);
        addChild(lockSprite);

        return true;
    }

    void FractionBarLayer::createFractionBar(int bottomValue) {
        int left = 0;
        int top = 0;
        int bottom = 0;
        if (bottomValue == 1) {
            left = 1;
            bottom = 0;
        } else {
            top = 1;
            bottom = bottomValue;
        }

        float width = 1.0 / bottomValue * BarWidth;
        float posY = DefaultPosY - (ids % 4) * BarHeight;
        auto barView = FractionBarView::create(Size(width, BarHeight), Vec2(V_WIDTH / 2, posY), BarColors[bottomValue - 1], left, top, bottom);
        barView->setOnTouchCallback([this, barView](int id, int type, float x, float y) {
            if (type == 0) { //BEGAN
            } else if (type == 1) { //MOVE
                checkIsCrashed(barView); //检测碰撞
            } else if (type == 2) { //END
                checkIsAttach(barView); //检测吸附
            }
            onDraw(); //刷新碰撞框
            updateLockSprite(); //更新锁精灵
        });
        barView->setOnSelectCallback([this, barView]() {
            curFractionBarView = barView;
            setLockSpriteVisible(true);
        });
        doContainer->addChild(barView);

        fractionBarViewList.pushBack(barView);
        
        //创建相应的map---碰撞使用
        Vector<FractionBarView*> list;
        fractionBarViewMap.insert(make_pair(barView, list));

        ids++;
    }

    void FractionBarLayer::checkIsCrashed(FractionBarView* target) {
        if (target == nullptr) return;

        for (FractionBarView* view : fractionBarViewList) {
            if (view == target) continue;
            
            Vector<FractionBarView*> list1 = fractionBarViewMap.at(target);
            Vector<FractionBarView*> list2 = fractionBarViewMap.at(view);
            if (view->isIntersect(target)) { //碰撞了
                if (list1.find(view) == list1.end()) { //未在map中，互相加入
                    fractionBarViewMap.at(target).pushBack(view);
                    fractionBarViewMap.at(view).pushBack(target);
                }
            } else { //未碰撞
                if (list1.find(view) != list1.end()) { //在map中，互相移除
                    fractionBarViewMap.at(target).eraseObject(view);
                    fractionBarViewMap.at(view).eraseObject(target);
                }
            }
        }
    }

    void FractionBarLayer::checkIsAttach(FractionBarView* target) {
        if (target == nullptr) return;

        for (FractionBarView* view : fractionBarViewList) {
            if (view == target) continue;

            float dx = 0, dy = 0;
            bool isCrash = false;
            if (target->isCloseEnough(view, dx, dy, isCrash)) {
                target->setPosition(Vec2(target->getPositionX() + dx, target->getPositionY() + dy));
                if (!isCrash) { //去掉碰撞记录
                    if (fractionBarViewMap.at(target).find(view) != fractionBarViewMap.at(target).end()) {
                        fractionBarViewMap.at(target).eraseObject(view);
                        fractionBarViewMap.at(view).eraseObject(target);
                    }
                } else { //加入碰撞记录
                    if (fractionBarViewMap.at(target).find(view) == fractionBarViewMap.at(target).end()) {
                        fractionBarViewMap.at(target).pushBack(view);
                        fractionBarViewMap.at(view).pushBack(target);
                    }
                }
            }
        }
    }

    void FractionBarLayer::setLockSpriteVisible(bool visible) {
        lockSprite->setVisible(visible);
        if (visible && curFractionBarView != nullptr) {
            updateLockSprite();
        }
    }
    
    void FractionBarLayer::updateLockSprite() {
        if (curFractionBarView != nullptr) {
            lockSprite->setTexture(curFractionBarView->getLockSpriteTexture());
            lockSprite->setPosition(curFractionBarView->getPosition() + Vec2(0, curFractionBarView->getContentSize().height / 2 + 20));
        }
    }

    void FractionBarLayer::onDraw() {
        borderDraw->clear();

        map<FractionBarView*, Vector<FractionBarView*>>::iterator it;
        for (it = fractionBarViewMap.begin(); it != fractionBarViewMap.end(); it++) {
            FractionBarView* view = it->first;
            if (it->second.size() < 1) continue;

            Vec2 position = view->getPosition();
            Size size = view->getContentSize();
            Vec2 points[] = {
                    Vec2(position.x - size.width / 2, position.y - size.height / 2),
                    Vec2(position.x + size.width / 2, position.y - size.height / 2),
                    Vec2(position.x + size.width / 2, position.y + size.height / 2),
                    Vec2(position.x - size.width / 2, position.y + size.height / 2),
            };
            borderDraw->drawPoly(points, 4, true, Color4F::RED);
        }
    }
}
