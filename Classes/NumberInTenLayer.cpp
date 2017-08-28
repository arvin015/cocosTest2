//
// Created by Administrator on 2017/8/26.
//

#include "NumberInTenLayer.h"
#include "DragImageView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const Rect BigRect = Rect(100, 100, 500, 500); //大活动区域
const Rect SmallLeftRect = Rect(100, 100, 250, 500); //小活动左边区域
const Rect SmallRightRect = Rect(110, 100, 250, 500); //小活动区域右边区域

const int TotalPage = 2; //总共页数
const int PageShowNum = 3; //每页显示图标个数
const string DragImages[] = {"btn_a_0.png", "btn_a_1.png", "btn_a_2.png", "btn_b_0.png", "btn_b_1.png", "btn_b_2.png"};
const int OffsetH = 10;

NumberInTenLayer::NumberInTenLayer()
: eventListener(nullptr)
, isDragEnabled(true)
, prevBtn(nullptr)
, nextBtn(nullptr)
, currentPage(1)
, dragImageStr("")
, tempSprite(nullptr) {

}

NumberInTenLayer::~NumberInTenLayer() {

}

void NumberInTenLayer::onEnter() {
    BaseLayer::onEnter();

    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(NumberInTenLayer::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(NumberInTenLayer::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(NumberInTenLayer::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}
void NumberInTenLayer::onExit() {

    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }

    BaseLayer::onExit();
}

bool NumberInTenLayer::init() {
    if(!BaseLayer::init()) {
        return false;
    }

    prevBtn = Button::create();
    prevBtn->setTitleFontSize(24);
    prevBtn->setTitleColor(Color3B::BLACK);
    prevBtn->setTitleText("向前");
    prevBtn->setPosition(Vec2(V_WIDTH - 300, 80));
    prevBtn->setTag(PREV_BTN);
    prevBtn->addClickEventListener(CC_CALLBACK_1(NumberInTenLayer::onBtnClick, this));
    prevBtn->setEnabled(false);
    this->addChild(prevBtn);

    nextBtn = Button::create();
    nextBtn->setTitleFontSize(24);
    nextBtn->setTitleColor(Color3B::BLACK);
    nextBtn->setTitleText("向后");
    nextBtn->setPosition(Vec2(V_WIDTH - 200, 80));
    nextBtn->setTag(NEXT_BTN);
    nextBtn->addClickEventListener(CC_CALLBACK_1(NumberInTenLayer::onBtnClick, this));
    this->addChild(nextBtn);

    setDragIcons(currentPage);

    return true;
}

void NumberInTenLayer::setDragIcons(int page) {

    //清除上一批拖拽图标
    for(ImageView* dragImage : dragImageList) {
        dragImage->removeFromParent();
    }
    dragImageList.clear();

    //重新设置拖拽图标
    for(int i = 0; i < PageShowNum; i++) {
        string str = DragImages[PageShowNum * (page - 1) + i];
        ImageView* dragImage = ImageView::create(str);
        dragImage->setPosition(Vec2(V_WIDTH - 300, V_HEIGHT - 200 - (100 + OffsetH) * i));
        dragImage->setName(str);
        dragImage->setTouchEnabled(true);
        dragImage->setSwallowTouches(false);
        dragImage->addTouchEventListener([this, dragImage](Ref* pSender, Widget::TouchEventType eventType){
            if(!isDragEnabled) {
                return false;
            }
            if(eventType == Widget::TouchEventType::BEGAN) {
                this->dragImageStr = dragImage->getName(); //设置当前拖拽对象
            }
        });
        this->addChild(dragImage);

        dragImageList.pushBack(dragImage);
    }
}

void NumberInTenLayer::setHotRect(HotRectType type) {
    if(type == BIG) {
        hotRect = BigRect;
    } else {
        hotRect = SmallLeftRect;
    }

    //test 绘制热区
    DrawNode* drawNode = DrawNode::create();
    Vec2 points[4];
    points[0] = hotRect.origin;
    points[1] = Vec2(hotRect.getMinX(), hotRect.getMaxY());
    points[2] = Vec2(hotRect.getMaxX(), hotRect.getMaxY());
    points[3] = Vec2(hotRect.getMaxX(), hotRect.getMinY());
    drawNode->drawPolygon(points, 4, Color4F(0.0f, 0.0f, 0.0f, 0.0f), 2, Color4F::GREEN);
    this->addChild(drawNode);
}

bool NumberInTenLayer::onTouchBegan(Touch* touch, Event* event) {

    if(!isDragEnabled || touch->getID() != 0) {
        return false;
    }

    if(!dragImageStr.empty()) {
        tempSprite = createTempSprite(dragImageStr, touch->getLocation());
        this->addChild(tempSprite);
    }

    return true;
}

void NumberInTenLayer::onTouchMoved(Touch* touch, Event* event) {

    if(touch->getID() != 0) {
        return;
    }

    if(tempSprite) {
        tempSprite->setPosition(tempSprite->getPosition() + touch->getDelta());
    }
}

void NumberInTenLayer::onTouchEnded(Touch* touch, Event* event) {
    if(tempSprite) {

        if(checkInHotRect(tempSprite)) { //在热区中
            string imgStr = tempSprite->getName();
            DragImageView* dragImageView = createDragImageView(imgStr, tempSprite->getPosition());
            this->addChild(dragImageView);

            //加入Map集合中
            map<string, Vector<DragImageView*>>::iterator it = dragImageViewMap.find(imgStr);
            if(it != dragImageViewMap.end()) { //存在key值imgStr
                Vector<DragImageView*> dsList = it->second;
                dsList.pushBack(dragImageView);
            } else { //不存在key值imgStr
                Vector<DragImageView*> dsList;
                dsList.pushBack(dragImageView);
                dragImageViewMap.insert(make_pair(imgStr, dsList));
            }
        }

        tempSprite->removeFromParent();
        tempSprite = nullptr;
    }
    dragImageStr = "";
}

bool NumberInTenLayer::checkInHotRect(Sprite* dragSprite) {

    if(!hotRect.containsPoint(dragSprite->getPosition())) {
        return false;
    }

    checkIsOutBorder(dragSprite);

    return true;
}

void NumberInTenLayer::checkIsOutBorder(cocos2d::Node* dragNode) {

    //越界处理
    float leftX = hotRect.getMinX();
    float topY = hotRect.getMaxY();
    float rightX = hotRect.getMaxX();
    float bottomY = hotRect.getMinY();

    float desLeftX = dragNode->getPositionX() - dragNode->getContentSize().width / 2;
    float desTopY = dragNode->getPositionY() + dragNode->getContentSize().height / 2;
    float desRightX = dragNode->getPositionX() + dragNode->getContentSize().width / 2;
    float desBottomY = dragNode->getPositionY() - dragNode->getContentSize().height / 2;

    if(desLeftX < leftX) {
        dragNode->setPositionX(leftX + dragNode->getContentSize().width / 2);
    }

    if(desTopY > topY) {
        dragNode->setPositionY(topY - dragNode->getContentSize().height / 2);
    }

    if(desRightX > rightX) {
        dragNode->setPositionX(rightX - dragNode->getContentSize().width / 2);
    }

    if(desBottomY < bottomY) {
        dragNode->setPositionY(bottomY + dragNode->getContentSize().height / 2);
    }
}

void NumberInTenLayer::onBtnClick(cocos2d::Ref* pSender) {
    Button* button = static_cast<Button*>(pSender);
    if(button) {
        switch (button->getTag()) {
            case PREV_BTN: {
                currentPage--;
                setDragIcons(currentPage);
                nextBtn->setEnabled(true);

                if(currentPage == 1) {
                    prevBtn->setEnabled(false);
                }
                break;
            }
            case NEXT_BTN: {
                currentPage++;
                setDragIcons(currentPage);
                prevBtn->setEnabled(true);

                if(currentPage == TotalPage) {
                    nextBtn->setEnabled(false);
                }
                break;
            }
            default:
                break;
        }
    }
}

Sprite* NumberInTenLayer::createTempSprite(const string &picStr, const Vec2 &position) {
    if(picStr.empty()) {
        return nullptr;
    }

    Sprite* tempSprite = Sprite::create(picStr);
    tempSprite->setName(picStr);
    tempSprite->setPosition(position);

    return tempSprite;
}

DragImageView* NumberInTenLayer::createDragImageView(const string &picStr, const Vec2 &position) {
    if(picStr.empty()) {
        return nullptr;
    }

    DragImageView* dragImageView = DragImageView::create(picStr);
    dragImageView->setContentSize(Size(100, 100));
    dragImageView->setName(picStr);
    dragImageView->setPosition(position);
    dragImageView->setOnMoveEndedCallback([this](DragImageView* dragImageView1){
        checkIsOutBorder(dragImageView1);
    });

    return dragImageView;
}