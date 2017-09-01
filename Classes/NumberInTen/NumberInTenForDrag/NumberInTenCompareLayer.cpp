//
//  NumberInTenCompareLayer.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/30.
//
//

#include "NumberInTenCompareLayer.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const Rect BigRect = Rect(80, 180, 600, 400); //大活动区域
const int MaxGroups = 5; //最多组数

//舞动相关数据
const int PaddingH = 2; //舞动结果排列横间距
const int PaddingV = 22; //舞动结果排列竖间距
const int ResultSX = 80 + 30; //舞动结果排列开始X位置
const int ResultSY = 180 + 400 - 50; //舞动结果排列开始Y位置

NumberInTenCompareLayer::NumberInTenCompareLayer()
: BaseNumberInTenDragLayer() {
    setNumberInTenDragImpl(this);
}

NumberInTenCompareLayer::~NumberInTenCompareLayer() {

}

void NumberInTenCompareLayer::onEnter() {
    BaseNumberInTenDragLayer::onEnter();
}

void NumberInTenCompareLayer::onExit() {
    BaseNumberInTenDragLayer::onExit();
}

bool NumberInTenCompareLayer::init() {
    if(!BaseNumberInTenDragLayer::init()) {
        return false;
    }
    
    return true;
}

void NumberInTenCompareLayer::initData() {
    
    hotRect = BigRect;
    maxGroups = MaxGroups;
    saveFileName = CompareSaveFile;
    
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

void NumberInTenCompareLayer::playAnim() {
    
    int index = 0;
    map<string, Vector<DragImageView*>>::iterator it;
    for(it = dragImageViewMap.begin(); it != dragImageViewMap.end(); it++) {
        string key = it->first;
        Vector<DragImageView*> value = it->second;

        //除第一组外，其他组都设置半透明
        if(index != 0) {
            for(DragImageView* dragImage : value) {
                dragImage->setIsTranslucent(true);
            }
        }
        
        float posSX = ResultSX;
        float posSY = ResultSY - (IconHeight + PaddingV) * index;
        
        //播放一组动画
        playMoveAnimationForGroup(value, posSX, posSY, PaddingH, 0, key, totalSpendTime, true, [this](string key){ //每组动画播放后的回调
            
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
            
        });
        //统计之前播放动画的总时间
        totalSpendTime += value.size() * playAnimTime;
        
        index++;
    }
}

int NumberInTenCompareLayer::checkInHotRect(Sprite* dragSprite) {
    if(!hotRect.containsPoint(dragSprite->getPosition())) {
        return false;
    }
    
    checkIsOutBorder(dragSprite, hotRect);
    
    return LEFT_RECT; //返回在左边热区中
}

void NumberInTenCompareLayer::onDragImageViewTouchEnded(DragImageView* dragImageView) {
    checkIsOutBorder(dragImageView, hotRect);
}

bool NumberInTenCompareLayer::isMaxGroup(const string &filename) {
    if(dragImageViewMap.size() >= MaxGroups) {
        map<string, Vector<DragImageView*>>::iterator it = dragImageViewMap.find(filename);
        if(it == dragImageViewMap.end()) {
            return true;
        }
    }
    return false;
}

void NumberInTenCompareLayer::addDragImageToList(DragImageView* dragImage, const string &filename) {
    
    map<string, Vector<DragImageView*>>::iterator it = dragImageViewMap.find(filename);
    Vector<DragImageView*> dsList;
    
    if(it != dragImageViewMap.end()) { //存在key值imgStr
        dsList = it->second;
    }
    dsList.pushBack(dragImage);
    dragImageViewMap[filename] = dsList; //map一定要再次才有效
}

void NumberInTenCompareLayer::deleteDragImageFromList(DragImageView* dragImage, const string &filename) {
   
    map<string, Vector<DragImageView*>>::iterator it = dragImageViewMap.find(filename);
    if(it != dragImageViewMap.end()) {
        Vector<DragImageView*> dsList = it->second;
        dsList.eraseObject(dragImage);
        if(dsList.size() == 0) { //该组全部删除了，移出map
            dragImageViewMap.erase(it);
        } else {
            dragImageViewMap[filename] = dsList;
        }
    }
}

int NumberInTenCompareLayer::getNumInGroup(const string &filename) {
    
    map<string, Vector<DragImageView*>>::iterator it = dragImageViewMap.find(filename);
    if(it != dragImageViewMap.end()) {
        return (int)it->second.size();
    }
    return 0;
}

void NumberInTenCompareLayer::toJson(rapidjson::Document &json) {
    
    json.SetObject();
    
    BaseNumberInTenDragLayer::toJson(json);
    
    //活动图案集合
    rapidjson::Value dragArray(rapidjson::kArrayType);
    map<string, Vector<DragImageView*>>::iterator it;
    for(it = dragImageViewMap.begin(); it != dragImageViewMap.end(); it++) {
        Vector<DragImageView*> dragList = it->second;
        rapidjson::Value dragImageArray(rapidjson::kArrayType);
        for(DragImageView* dragImage : dragList) {
            rapidjson::Document dragObj(&json.GetAllocator());
            dragImage->toJson(dragObj);
            dragArray.PushBack(dragObj, json.GetAllocator());
        }
    }
    
    json.AddMember("dragImages", dragArray, json.GetAllocator());
}
