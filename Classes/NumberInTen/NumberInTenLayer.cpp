//
// Created by Administrator on 2017/8/26.
//

#include "NumberInTenLayer.h"
#include "DragImageView.h"
#include "PlayImageView.h"
#include "PacToast.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const Rect BigRect = Rect(80, 180, 600, 400); //大活动区域
const Rect SmallLeftRect = Rect(80, 180, 295, 400); //小活动左边区域
const Rect SmallRightRect = Rect(90, 180, 295, 400); //小活动区域右边区域

const int IconWidth = 58;
const int IconHeight = 58;
const int TotalAddNumPer = 10; //每种图案最多能创建个数
const int TotalPage = 2; //总共页数
const int PageShowNum = 5; //每页显示图案个数
const int TotalIconNum = 10; //总图案个数
const string NormalDragImages[] = {"zero.png", "one.png", "two.png", "three.png", "four.png", "five.png", "six.png", "seven.png", "eight.png", "nine.png"};
const string DisableDragImages[] = {"zero_d.png", "one_d.png", "two_d.png", "three_d.png", "four_d.png", "five_d.png", "six_d.png", "seven_d.png", "eight_d.png", "nine_d.png"};
const int OffsetH = 10; //图案间纵间距
const int MaxGroups = 5; //最多组数

//舞动相关数据
const int PaddingH = 2; //舞动结果排列横间距
const int PaddingV = 22; //舞动结果排列竖间距
const int ResultSX = 80 + 30; //舞动结果排列开始X位置
const int ResultSY = 180 + 400 - 50; //舞动结果排列开始Y位置
float playAnimTime = 1.0f; //每个图案动画播放时间

NumberInTenLayer::NumberInTenLayer()
: eventListener(nullptr)
, isDragEnabled(true)
, prevBtn(nullptr)
, nextBtn(nullptr)
, danceBtn(nullptr)
, currentPage(1)
, seletedImage(nullptr)
, tempSprite(nullptr)
, totalSpendTime(0.0f)
, needSetAlpha(true){

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
    
    danceBtn = Button::create();
    danceBtn->setTitleFontSize(24);
    danceBtn->setTitleColor(Color3B::BLACK);
    danceBtn->setTitleText("舞動");
    danceBtn->setPosition(Vec2(V_WIDTH - 100, 300));
    danceBtn->setTag(DANCE_BTN);
    danceBtn->addClickEventListener(CC_CALLBACK_1(NumberInTenLayer::onBtnClick, this));
    this->addChild(danceBtn);

    initDragIcons();
    switchDragIcons(currentPage);

    return true;
}

void NumberInTenLayer::initDragIcons() {

    for(int i = 0; i < TotalIconNum; i++) {
        string str = NormalDragImages[i];
        PlayImageView* dragImage = PlayImageView::create(str, DisableDragImages[i]);
        dragImage->setContentSize(Size(IconWidth, IconHeight));
        dragImage->setScale9Enabled(true);
        dragImage->setName(str); //设置图片名
        dragImage->setTag(i / PageShowNum + 1); //设置页码
        dragImage->setTouchEnabled(true);
        dragImage->addTouchEventListener([this, dragImage](Ref* pSender, Widget::TouchEventType eventType){
            if(!isDragEnabled) {
                return;
            }
            if(eventType == Widget::TouchEventType::BEGAN) {
                this->seletedImage = dragImage; //设置当前拖拽对象
            }
        });
        dragImage->setVisible(false);
        this->addChild(dragImage);

        dragImageList.pushBack(dragImage);
    }
}

void NumberInTenLayer::switchDragIcons(int page) {
    int count = 0;
    for(int i = 0; i < dragImageList.size(); i++) {
        ImageView* playImage = dragImageList.at(i);
        if(playImage->getTag() == page) {
            playImage->setVisible(true);
            playImage->setPosition(Vec2(V_WIDTH - 260, V_HEIGHT - 200 - (IconHeight + OffsetH) * count));
            playImage->setSwallowTouches(false); //很关键，在创建时设置会无效，一定要在设置setVisible后再设置才有效
            count++;
        } else {
            playImage->setVisible(false);
            playImage->setPosition(Vec2(V_WIDTH + 100, V_HEIGHT + 100));
        }
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

void NumberInTenLayer::dance() {
    
    //播放动画
    int index = 0;
    map<string, Vector<DragImageView*>>::iterator it;
    for(it = dragImageViewMap.begin(); it != dragImageViewMap.end(); it++) {
        string key = it->first;
        Vector<DragImageView*> value = it->second;

        //需要设置透明度
        if(needSetAlpha) {
            //除第一组外，其他组都设置半透明
            if(index != 0) {
                for(DragImageView* dragImage : value) {
                    dragImage->setIsTranslucent(true);
                }
            }
        }
        
        //播放一组动画
        playMoveAnimationForGroup(value, index, key, totalSpendTime, [this](string key){ //每组动画播放后的回调
            //需要设置透明度
            if(needSetAlpha) {
                //关键是获取该组的下一组，将下一组图案取消半透明
                map<string, Vector<DragImageView*>>::iterator it1 = dragImageViewMap.find(key);
                it1++;
                if(it1 != dragImageViewMap.end()) {
                    string key1 = it1->first;
                    Vector<DragImageView*> value1 = it1->second;
                    for(DragImageView* dragImage : value1) {
                        dragImage->setIsTranslucent(false);
                    }
                }
            }
        });
        //统计之前播放动画的总时间
        totalSpendTime += value.size() * playAnimTime;
        
        index++;
    }
    
    //禁止所有图案的操作
    forbidIconsToDrag();
}

bool NumberInTenLayer::onTouchBegan(Touch* touch, Event* event) {

    if(!isDragEnabled || touch->getID() != 0) {
        return false;
    }

    if(seletedImage) {
        
        //判断是否达到了最大可放组
        if(isMaxGroup(seletedImage->getName())) {
            PacToast::makeText(StringUtils::format("最多只能放入%d組圖案", MaxGroups));
            seletedImage = nullptr;
            return false;
        }
        
        tempSprite = createTempSprite(seletedImage->getName(), touch->getLocation());
        this->addChild(tempSprite, getMaxZOrder() + 1);
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
            this->addChild(dragImageView, getMaxZOrder() + 1);

            //加入Map集合中
            map<string, Vector<DragImageView*>>::iterator it = dragImageViewMap.find(imgStr);
            Vector<DragImageView*> dsList;
            
            if(it != dragImageViewMap.end()) { //存在key值imgStr
                dsList = it->second;
            }
            dsList.pushBack(dragImageView);
            dragImageViewMap[imgStr] = dsList; //map一定要再次才有效
            
            if(dsList.size() >= TotalAddNumPer) { //达到可加入的最大值，禁止再添加
                seletedImage->setEnabled(false); //不可加入了，这里切记不能设置setTouchEnabled(false)，否则会阻断父类的Touch事件
            }
        }

        tempSprite->removeFromParent();
        tempSprite = nullptr;
        
        seletedImage = nullptr;
    }
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
                switchDragIcons(currentPage);
                nextBtn->setEnabled(true);

                if(currentPage == 1) {
                    prevBtn->setEnabled(false);
                }
                break;
            }
            case NEXT_BTN: {
                currentPage++;
                switchDragIcons(currentPage);
                prevBtn->setEnabled(true);

                if(currentPage == TotalPage) {
                    nextBtn->setEnabled(false);
                }
                break;
            }
            case DANCE_BTN: {
                dance();
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
    dragImageView->setContentSize(Size(IconWidth, IconHeight));
    dragImageView->setName(picStr);
    dragImageView->setPosition(position);
    dragImageView->setOnMoveEndedCallback([this, dragImageView](){
        checkIsOutBorder(dragImageView);
    });
    dragImageView->setOnDeleteCallback([this, dragImageView, picStr](){
        
        //从map集合中删掉
        map<string, Vector<DragImageView*>>::iterator it = dragImageViewMap.find(picStr);
        if(it != dragImageViewMap.end()) {
            Vector<DragImageView*> dsList = it->second;
            dsList.eraseObject(dragImageView);
            if(dsList.size() == 0) { //该组全部删除了，移出map
                dragImageViewMap.erase(it);
            } else {
                dragImageViewMap[picStr] = dsList;
            }
        }
        
        //如果该图案不能Touch了，恢复它可Touch
        ImageView* image = static_cast<ImageView*>(this->getChildByName(picStr));
        if(image) {
            if(!image->isEnabled()) {
                image->setEnabled(true);
            }
        }
    });
    
    return dragImageView;
}

int NumberInTenLayer::getMaxZOrder() {
    Vector<Node*> nodeList = getChildren();
    Node* node = nodeList.at(getChildrenCount() - 1); //获取最上面的那个Node
    return node->getLocalZOrder();
}

void NumberInTenLayer::playMoveAnimationForGroup(Vector<DragImageView*> dragImageList, int index, const string &keyOfFileName, float delayTime, const function<void(string)> &animactionOver)
{
    
    float posX = ResultSX;
    float posY = ResultSY - (IconHeight + PaddingV) * index;
    
    Vector<FiniteTimeAction*> actionList;
    
    //先加入延时动画，目的使每组动画能先后播放
    auto delayAction = DelayTime::create(delayTime);
    actionList.pushBack(delayAction);
    
    for(int i = 0; i < dragImageList.size(); i++) {
        
        DragImageView* dragImage = dragImageList.at(i);
        
        posX = ResultSX + (IconWidth + PaddingH) * i;
        
        auto moveAction = Sequence::create(MoveTo::create(1.0f, Vec2(posX, posY)), CallFunc::create([dragImage, i](){
            dragImage->addShowNum((i + 1)); //单个图案播放完后显示数字
        }), NULL);
        auto targetAction = TargetedAction::create(dragImage, moveAction);
        
        actionList.pushBack(targetAction);
    }
    
    auto endCall = CallFunc::create([animactionOver, keyOfFileName](){
        if(animactionOver) {
            animactionOver(keyOfFileName); //需返回该组Key值，为了区分组
        }
    });
    actionList.pushBack(endCall);
    
    auto animation = Sequence::create(actionList);
    dragImageList.at(0)->runAction(animation);
}

void NumberInTenLayer::forbidIconsToDrag() {
    for(ImageView* dragImage : dragImageList) {
        dragImage->setEnabled(false);
    }
}

bool NumberInTenLayer::isMaxGroup(const string &filename) {
    if(dragImageViewMap.size() >= MaxGroups) {
        map<string, Vector<DragImageView*>>::iterator it = dragImageViewMap.find(filename);
        if(it == dragImageViewMap.end()) {
            return true;
        }
    }
    return false;
}

