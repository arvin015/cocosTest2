//
// Created by Administrator on 2017/8/30.
//

#include "BaseNumberInTenDragLayer.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const int TotalAddNumPer = 10; //每种图案最多能创建个数
const int TotalPage = 2; //总共页数
const int PageShowNum = 5; //每页显示图案个数
const int TotalIconNum = 10; //总图案个数
const string NormalDragImages[] = {"zero.png", "one.png", "two.png", "three.png", "four.png", "five.png", "six.png", "seven.png", "eight.png", "nine.png"};
const string DisableDragImages[] = {"zero_d.png", "one_d.png", "two_d.png", "three_d.png", "four_d.png", "five_d.png", "six_d.png", "seven_d.png", "eight_d.png", "nine_d.png"};
const int OffsetH = 10; //图案间纵间距

BaseNumberInTenDragLayer::BaseNumberInTenDragLayer()
: eventListener(nullptr)
, isDragEnabled(true)
, prevBtn(nullptr)
, nextBtn(nullptr)
, danceBtn(nullptr)
, currentPage(1)
, seletedImage(nullptr)
, tempSprite(nullptr)
, playAnimTime(1.0f)
, totalSpendTime(0.0f)
, numberInTenDragImpl(nullptr)
, isDanced(false) {

}

BaseNumberInTenDragLayer::~BaseNumberInTenDragLayer() {

}

void BaseNumberInTenDragLayer::onEnter() {
    BaseLayer::onEnter();

    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(BaseNumberInTenDragLayer::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(BaseNumberInTenDragLayer::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(BaseNumberInTenDragLayer::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}
void BaseNumberInTenDragLayer::onExit() {

    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }

    BaseLayer::onExit();
}

bool BaseNumberInTenDragLayer::init() {
    if(!BaseLayer::init()) {
        return false;
    }

    prevBtn = Button::create();
    prevBtn->setTitleFontSize(24);
    prevBtn->setTitleColor(Color3B::BLACK);
    prevBtn->setTitleText("向前");
    prevBtn->setPosition(Vec2(V_WIDTH - 300, 80));
    prevBtn->setTag(PREV_BTN);
    prevBtn->addClickEventListener(CC_CALLBACK_1(BaseNumberInTenDragLayer::onBtnClick, this));
    prevBtn->setEnabled(false);
    this->addChild(prevBtn);

    nextBtn = Button::create();
    nextBtn->setTitleFontSize(24);
    nextBtn->setTitleColor(Color3B::BLACK);
    nextBtn->setTitleText("向后");
    nextBtn->setPosition(Vec2(V_WIDTH - 200, 80));
    nextBtn->setTag(NEXT_BTN);
    nextBtn->addClickEventListener(CC_CALLBACK_1(BaseNumberInTenDragLayer::onBtnClick, this));
    this->addChild(nextBtn);
    
    danceBtn = Button::create();
    danceBtn->setTitleFontSize(24);
    danceBtn->setTitleColor(Color3B::BLACK);
    danceBtn->setTitleText("舞動");
    danceBtn->setPosition(Vec2(V_WIDTH - 100, 300));
    danceBtn->setTag(DANCE_BTN);
    danceBtn->addClickEventListener(CC_CALLBACK_1(BaseNumberInTenDragLayer::onBtnClick, this));
    this->addChild(danceBtn);

    if(numberInTenDragImpl) {
        numberInTenDragImpl->initData(); //初始化数据
    }
    initDragIcons();
    switchDragIcons(currentPage);
    
    string result = FileUtils::getInstance()->getStringFromFile(FileUtils::getInstance()->getWritablePath() + "/" + saveFileName);
    rapidjson::Document doc;
    doc.Parse(result.c_str());
    fromJson(doc);

    return true;
}

void BaseNumberInTenDragLayer::initDragIcons() {

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

void BaseNumberInTenDragLayer::switchDragIcons(int page) {
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

void BaseNumberInTenDragLayer::onBackHandle() {
    rapidjson::Document doc(rapidjson::kObjectType);
    toJson(doc);
    string result = getStringFromJson(doc);
    FileUtils::getInstance()->writeStringToFile(result, FileUtils::getInstance()->getWritablePath() + "/" + saveFileName);
}

void BaseNumberInTenDragLayer::dance() {
    
    isDanced = true;
    
    //播放动画，交给子类处理
    if(numberInTenDragImpl) {
        numberInTenDragImpl->playAnim();
    }
    
    //禁止所有图案的操作
    setDragImagesEnabled(false);
}

void BaseNumberInTenDragLayer::fromJson(const rapidjson::Document &doc) {
    
    if(!doc.IsObject()) {
        return;
    }
    
    //操作栏图案
    if(doc.HasMember("playImages")) {
        const rapidjson::Value &playArray = doc["playImages"];
        for(rapidjson::SizeType i = 0; i < playArray.Size(); i++) {
            bool flag = playArray[i].GetBool();
            PlayImageView* playImage = dragImageList.at(i);
            if(playImage) {
                playImage->setEnabled(flag);
            }
        }
    }
    
    //拖拽图案
    if(doc.HasMember("dragImages")) {
        const rapidjson::Value &dragArray = doc["dragImages"];
        if(dragArray.IsArray()) {
            for(rapidjson::SizeType i = 0; i < dragArray.Size(); i++) {
                const rapidjson::Value &dragObj = dragArray[i];
                if(dragObj.IsObject()) {
                    string filename = dragObj["image"].GetString();
                    DragImageView* dragImage = createDragImageView(filename, Vec2::ZERO);
                    dragImage->fromJson(dragObj);
                    this->addChild(dragImage);
                    
                    if(numberInTenDragImpl) {
                        numberInTenDragImpl->addDragImageToList(dragImage, filename); //加入集合中
                    }
                }
            }
        }
    }
    
    if(doc.HasMember("isDanced")) {
        isDanced = doc["isDanced"].GetBool();
        if(isDanced) {
            setDragImagesEnabled(false);
        }
    }
}

void BaseNumberInTenDragLayer::toJson(rapidjson::Document &json) {
    json.AddMember("isDanced", isDanced, json.GetAllocator());
    
    //操作栏图案状态
    rapidjson::Value playArray(rapidjson::kArrayType);
    for(PlayImageView* imageView : dragImageList) {
        playArray.PushBack(imageView->isEnabled(), json.GetAllocator());
    }
    json.AddMember("playImages", playArray, json.GetAllocator());
}

bool BaseNumberInTenDragLayer::onTouchBegan(Touch* touch, Event* event) {

    if(!isDragEnabled || touch->getID() != 0) {
        return false;
    }

    if(seletedImage) {
        
        //判断是否达到了最大可放组
        if(numberInTenDragImpl != nullptr && numberInTenDragImpl->isMaxGroup(seletedImage->getName())) {
            PacToast::makeText(StringUtils::format("最多只能放入%d組圖案", maxGroups));
            seletedImage = nullptr;
            return false;
        }
        
        tempSprite = createTempSprite(seletedImage->getName(), touch->getLocation());
        this->addChild(tempSprite, getMaxZOrder() + 1);
    }

    return true;
}

void BaseNumberInTenDragLayer::onTouchMoved(Touch* touch, Event* event) {

    if(touch->getID() != 0) {
        return;
    }

    if(tempSprite) {
        tempSprite->setPosition(tempSprite->getPosition() + touch->getDelta());
    }
}

void BaseNumberInTenDragLayer::onTouchEnded(Touch* touch, Event* event) {
    
    if(tempSprite) {

        int rectType = numberInTenDragImpl->checkInHotRect(tempSprite);
        
        if(rectType) { //在热区中

            string imgStr = tempSprite->getName();
            DragImageView* dragImageView = createDragImageView(imgStr, tempSprite->getPosition());
            dragImageView->setTag(rectType);
            this->addChild(dragImageView, getMaxZOrder() + 1);

            //加入集合中
            if(numberInTenDragImpl) {
                numberInTenDragImpl->addDragImageToList(dragImageView, imgStr);
            }
            
            if(numberInTenDragImpl && numberInTenDragImpl->getNumInGroup(imgStr) >= TotalAddNumPer) { //达到可加入的最大值，禁止再添加
                seletedImage->setEnabled(false); //不可加入了，这里切记不能设置setTouchEnabled(false)，否则会阻断父类的Touch事件
            }
        }

        tempSprite->removeFromParent();
        tempSprite = nullptr;
        
        seletedImage = nullptr;
    }
}

void BaseNumberInTenDragLayer::onBtnClick(cocos2d::Ref* pSender) {
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

Sprite* BaseNumberInTenDragLayer::createTempSprite(const string &picStr, const Vec2 &position) {
    if(picStr.empty()) {
        return nullptr;
    }

    Sprite* tempSprite = Sprite::create(picStr);
    tempSprite->setName(picStr);
    tempSprite->setPosition(position);

    return tempSprite;
}

DragImageView* BaseNumberInTenDragLayer::createDragImageView(const string &picStr, const Vec2 &position) {

    DragImageView* dragImageView = DragImageView::create(picStr);
    dragImageView->setContentSize(Size(IconWidth, IconHeight));
    dragImageView->setName(picStr);
    dragImageView->setPosition(position);
    dragImageView->setOnMoveEndedCallback([this, dragImageView](){
        if(numberInTenDragImpl) {
            numberInTenDragImpl->onDragImageViewTouchEnded(dragImageView);
        }
    });
    dragImageView->setOnDeleteCallback([this, dragImageView, picStr](){
        
        //从集合中移除
        if(numberInTenDragImpl) {
            numberInTenDragImpl->deleteDragImageFromList(dragImageView, picStr);
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

void BaseNumberInTenDragLayer::checkIsOutBorder(Node* dragNode, const Rect &rect) {
    
    //越界处理
    float leftX = rect.getMinX();
    float topY = rect.getMaxY();
    float rightX = rect.getMaxX();
    float bottomY = rect.getMinY();
    
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

int BaseNumberInTenDragLayer::getMaxZOrder() {
    Vector<Node*> nodeList = getChildren();
    Node* node = nodeList.at(getChildrenCount() - 1); //获取最上面的那个Node
    return node->getLocalZOrder();
}

void BaseNumberInTenDragLayer::playMoveAnimationForGroup(Vector<DragImageView*> dragImageList, float posSX, float posSY, float paddingH, float paddingV, const string &keyOfFileName, float delayTime, bool isCrosswise, const function<void(string)> &animactionOver)
{
    
    Vector<FiniteTimeAction*> actionList;
    
    //先加入延时动画，目的使每组动画能先后播放
    auto delayAction = DelayTime::create(delayTime);
    actionList.pushBack(delayAction);

    for(int i = 0; i < dragImageList.size(); i++) {
        
        DragImageView* dragImage = dragImageList.at(i);
        
        float posX;
        float posY;
        if(isCrosswise) { //横向排列---考虑换行情况
            int maxNumInRow = 10; //一行最多可排列个数
            if(keyOfFileName.empty()) {
                maxNumInRow = 5; //数的组合是五个
            }
            posX = posSX + (IconWidth + paddingH) * (i % maxNumInRow);
            posY = posSY - (IconHeight + paddingV) * (i  / maxNumInRow);
        } else { //纵向排列
            posX = posSX + i / 2 * (IconWidth + paddingH);
            posY = posSY - i % 2 * (IconHeight + paddingV);
        }
        
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
    this->runAction(animation);
}

void BaseNumberInTenDragLayer::setDragImagesEnabled(bool enabled) {
    for(ImageView* dragImage : dragImageList) {
        dragImage->setEnabled(enabled);
    }
}

void BaseNumberInTenDragLayer::forbidIconsToDrag(const string &filename) {
    for(ImageView* dragImage : dragImageList) {
        if(strcmp(dragImage->getName().c_str(), filename.c_str()) == 0) {
            dragImage->setEnabled(true);
        } else {
            dragImage->setEnabled(false);
        }
    }
}

