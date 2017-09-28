//
//  NumberInTenFlyLayer.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/31.
//
//

#include "NumberInTenFlyLayer.h"
#include "FlyView.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include "CommonUtils.h"
#include "PacToast.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const string SaveFileName = "numberInTenFly.txt";

NumberInTenFlyLayer::NumberInTenFlyLayer()
: flyView(nullptr)
, isDanced(false) {

}

NumberInTenFlyLayer::~NumberInTenFlyLayer() {

}

void NumberInTenFlyLayer::onEnter() {
    BaseLayer::onEnter();
}

void NumberInTenFlyLayer::onExit() {

    BaseLayer::onExit();
}

bool NumberInTenFlyLayer::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    flyView = FlyView::create();
    flyView->setPosition(Vec2(100, V_HEIGHT / 2 + 20));
    this->addChild(flyView);
    
    Button* danceBtn = Button::create();
    danceBtn->setTitleFontSize(24);
    danceBtn->setTitleColor(Color3B::BLACK);
    danceBtn->setTitleText("舞動");
    danceBtn->setPosition(Vec2(V_WIDTH - 100, 300));
    danceBtn->addClickEventListener([this](Ref* pSender){
        if(isDanced) {
            PacToast::makeText("該對象已舞動");
            return;
        }
        
        if(flyView->checkCanDance()) {
            isDanced = true;
            flyView->dance();
        }
    });
    this->addChild(danceBtn);
    
    Button* restoreBtn = Button::create();
    restoreBtn->setTitleFontSize(24);
    restoreBtn->setTitleColor(Color3B::BLACK);
    restoreBtn->setTitleText("恢復");
    restoreBtn->setPosition(Vec2(V_WIDTH - 100, 200));
    restoreBtn->addClickEventListener([this](Ref* pSender){
        string result = FileUtils::getInstance()->getStringFromFile(FileUtils::getInstance()->getWritablePath() + "/" + SaveFileName);
        rapidjson::Document doc;
        doc.Parse(result.c_str());
        fromJson(doc);
    });
    this->addChild(restoreBtn);
    
    return true;
}

void NumberInTenFlyLayer::onBackHandle() {
    rapidjson::Document doc(rapidjson::kObjectType);
    toJson(doc);
    string result = getStringFromJson(doc);
    FileUtils::getInstance()->writeStringToFile(result, FileUtils::getInstance()->getWritablePath() + "/" + SaveFileName);
}

void NumberInTenFlyLayer::fromJson(const rapidjson::Value &json) {
    if(!json.IsObject()) {
        return;
    }
    
    if(json.HasMember("isDanced")) {
        isDanced = json["isDanced"].GetBool();
    }
    
    if(flyView) {
        flyView->fromJson(json);
    }
}

void NumberInTenFlyLayer::toJson(rapidjson::Document &json) {
    json.AddMember("isDanced", isDanced, json.GetAllocator());
    
    if(flyView) {
        flyView->toJson(json);
    }
}

