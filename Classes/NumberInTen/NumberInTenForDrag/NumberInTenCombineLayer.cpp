//
//  NumberInTenCombineLayer.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/30.
//
//

#include "NumberInTenCombineLayer.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const Rect SmallLeftRect = Rect(80, 180, 295, 400); //小活动左边区域
const Rect SmallRightRect = Rect(390, 180, 295, 400); //小活动区域右边区域
const int MaxGroups = 1; //最多组数

//舞动相关数据
const int PaddingH = 2; //舞动结果排列横间距
const int PaddingV = 62; //舞动结果排列竖间距
const int LeftResultSX = 80 + 30; //舞动结果排列左边开始X位置
const int LeftResultSY = 180 + 400 - 50; //舞动结果排列左边开始Y位置
const int RightResultSX = 390 + 30; //舞动结果排列左边开始X位置
const int RightResultSY = 180 + 400 - 50; //舞动结果排列左边开始Y位置

NumberInTenCombineLayer::NumberInTenCombineLayer()
: BaseNumberInTenDragLayer() {
    setNumberInTenDragImpl(this);
}

NumberInTenCombineLayer::~NumberInTenCombineLayer() {

}

void NumberInTenCombineLayer::onEnter() {
    BaseNumberInTenDragLayer::onEnter();
}

void NumberInTenCombineLayer::onExit() {
    BaseNumberInTenDragLayer::onExit();
}

bool NumberInTenCombineLayer::init() {
    if(!BaseNumberInTenDragLayer::init()) {
        return false;
    }
    
    return true;
}

void NumberInTenCombineLayer::initData() {
    
    Vector<DragImageView*> leftDragList;
    Vector<DragImageView*> rightDragList;
    
    hotRectMap.insert(make_pair(LEFT_RECT, SmallLeftRect));
    hotRectMap.insert(make_pair(RIGHT_RECT, SmallRightRect));
    dragImageListMap.insert(make_pair(LEFT_RECT, leftDragList));
    dragImageListMap.insert(make_pair(RIGHT_RECT, rightDragList));
    maxGroups = MaxGroups;
    saveFileName = CombineSaveFile;
    
    //test 绘制热区
    DrawNode* drawNode = DrawNode::create();
    Vec2 points[4];
    points[0] = SmallLeftRect.origin;
    points[1] = Vec2(SmallLeftRect.getMinX(), SmallLeftRect.getMaxY());
    points[2] = Vec2(SmallLeftRect.getMaxX(), SmallLeftRect.getMaxY());
    points[3] = Vec2(SmallLeftRect.getMaxX(), SmallLeftRect.getMinY());
    drawNode->drawPolygon(points, 4, Color4F(0.0f, 0.0f, 0.0f, 0.0f), 2, Color4F::GREEN);
    this->addChild(drawNode);
    
    DrawNode* drawNode1 = DrawNode::create();
    Vec2 points1[4];
    points1[0] = SmallRightRect.origin;
    points1[1] = Vec2(SmallRightRect.getMinX(), SmallRightRect.getMaxY());
    points1[2] = Vec2(SmallRightRect.getMaxX(), SmallRightRect.getMaxY());
    points1[3] = Vec2(SmallRightRect.getMaxX(), SmallRightRect.getMinY());
    drawNode1->drawPolygon(points1, 4, Color4F(0.0f, 0.0f, 0.0f, 0.0f), 2, Color4F::GREEN);
    this->addChild(drawNode1);
}

void NumberInTenCombineLayer::playAnim() {
    
    //播放左边动画
    map<int, Vector<DragImageView*>>::iterator it1 = dragImageListMap.find(LEFT_RECT);
    Vector<DragImageView*> dragList1 = it1->second;
    playMoveAnimationForGroup(dragList1, LeftResultSX, LeftResultSY, PaddingH, PaddingV, "", 0, true, nullptr);

    //播放右边动画
    totalSpendTime = dragList1.size() * playAnimTime; //计算右边播放动画需要的延时
    map<int, Vector<DragImageView*>>::iterator it2 = dragImageListMap.find(RIGHT_RECT);
    Vector<DragImageView*> dragList2 = it2->second;
    playMoveAnimationForGroup(dragList2, RightResultSX, RightResultSY, PaddingH, PaddingV, "", totalSpendTime, true, [this](string filename){ //动画结束后显示下方统计数字
        
        Vector<FiniteTimeAction*> animList;
        
        //添加透明的统计数字
        int count = 1;
        map<int, Vector<DragImageView*>>::iterator it;
        for(it = dragImageListMap.begin(); it != dragImageListMap.end(); it++) {
            
            Vector<DragImageView*> value = it->second;
            for(DragImageView* dragImage : value) {
                dragImage->addShowCountNum(count, 0);
                
                animList.pushBack(DelayTime::create(count * 0.1f));
                animList.pushBack(TargetedAction::create(dragImage->getCountText(), FadeIn::create(0.5f)));
                
                count++;
            }
        }
        
        auto endCall = CallFunc::create([](){ //动画结束回调
        });
        animList.pushBack(endCall);
        
        //播放动画逐个显示
        auto animation = Sequence::create(animList);
        this->runAction(animation);
    });
}

int NumberInTenCombineLayer::checkInHotRect(Sprite* dragSprite) {
    
    int result = false;
    Rect hotRect;
    
    map<int, Rect>::iterator it;
    for(it = hotRectMap.begin(); it != hotRectMap.end(); it++) {
        Rect rect = it->second;
        if(rect.containsPoint(dragSprite->getPosition())) {
            result = it->first;
            hotRect = rect;
            break;
        }
    }
    
    if(result) { //在热区中
        dragSprite->setTag(result); //设置热区编号
        checkIsOutBorder(dragSprite, hotRect); //越界检测
    }
    
    return result;
}

void NumberInTenCombineLayer::onDragImageViewTouchEnded(DragImageView* dragImageView) {
    
    int lastRectType = dragImageView->getTag(); //拖拽前所属热区
    int currentRectType; //当前所属热区
    
    Rect lastHotRect; //拖拽前所在热区
    Rect currentRect; //当前所在热区
    
    map<int, Rect>::iterator it = hotRectMap.find(lastRectType);
    if(it != hotRectMap.end()) {
        lastHotRect = it->second;
    }
    
    //获取当前所在热区
    Vec2 currentPos = dragImageView->getPosition(); //拖拽放手时的位置
    bool isInHot = false;
    map<int, Rect>::iterator it1;
    for(it1 = hotRectMap.begin(); it1 != hotRectMap.end(); it1++) {
        Rect rect = it1->second;
        if(rect.containsPoint(currentPos)) {
            currentRectType = it1->first;
            currentRect = rect;
            isInHot = true;
            break;
        }
    }
    
    //当前不在热区中
    if(!isInHot) {
        checkIsOutBorder(dragImageView, lastHotRect); //回到原有热区
        return;
    }
    
    if(lastRectType != currentRectType) { //改变了热区
        map<int, Vector<DragImageView*>>::iterator it2 = dragImageListMap.find(lastRectType);
        it2->second.eraseObject(dragImageView);
        
        map<int, Vector<DragImageView*>>::iterator it3 = dragImageListMap.find(currentRectType);
        it3->second.pushBack(dragImageView);
        
        dragImageView->setTag(currentRectType); //重设当前所在热区
    }
    
    checkIsOutBorder(dragImageView, currentRect); //是否越界
}

bool NumberInTenCombineLayer::isMaxGroup(const string &filename) {
    return false;
}

void NumberInTenCombineLayer::addDragImageToList(DragImageView* dragImage, const string &filename) {
    
    //设置除filename图案外，其余都灰掉
    if(getNumInGroup(filename) == 0) {
        forbidIconsToDrag(filename);
    }
    
    int rectType = dragImage->getTag();
    
    Vector<DragImageView*> dragList;
    map<int, Vector<DragImageView*>>::iterator it = dragImageListMap.find(rectType);
    dragList = it->second;
    dragList.pushBack(dragImage);
    dragImageListMap[rectType] = dragList;
}

void NumberInTenCombineLayer::deleteDragImageFromList(DragImageView* dragImage, const string &filename) {
    int rectType = dragImage->getTag();
    
    Vector<DragImageView*> dragList;
    map<int, Vector<DragImageView*>>::iterator it = dragImageListMap.find(rectType);
    dragList = it->second;
    dragList.eraseObject(dragImage);
    dragImageListMap[rectType] = dragList;
    
    //热区中没有图案了，所有操作栏图案重新可用
    if(getNumInGroup(filename) == 0) {
        setDragImagesEnabled(true);
    }
}

int NumberInTenCombineLayer::getNumInGroup(const string &filename) {
    
    int count = 0;
    
    map<int, Vector<DragImageView*>>::iterator it1 = dragImageListMap.find(LEFT_RECT);
    count += it1->second.size();
    
    map<int, Vector<DragImageView*>>::iterator it2 = dragImageListMap.find(RIGHT_RECT);
    count += it2->second.size();
    
    return count;
}

void NumberInTenCombineLayer::toJson(rapidjson::Document &json) {
    
    json.SetObject();
    
    BaseNumberInTenDragLayer::toJson(json);
    
    //活动图案集合
    rapidjson::Value dragArray(rapidjson::kArrayType);
    map<int, Vector<DragImageView*>>::iterator it;
    for(it = dragImageListMap.begin(); it != dragImageListMap.end(); it++) {
        Vector<DragImageView*> dragList = it->second;
        for(DragImageView* dragImage : dragList) {
            rapidjson::Document dragObj(&json.GetAllocator());
            dragImage->toJson(dragObj);
            dragArray.PushBack(dragObj, json.GetAllocator());
        }
    }
    
    json.AddMember("dragImages", dragArray, json.GetAllocator());
}
