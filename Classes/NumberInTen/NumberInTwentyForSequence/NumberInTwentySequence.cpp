//
//  NumberInTwentySequence.cpp
//  CocosTest2
//
//  Created by arvin on 2017/9/27.
//
//

#include "NumberInTwentySequence.h"
#include "SequenceView.h"
#include "ui/UIButton.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include "CommonUtils.h"
#include "PacToast.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const string SaveFileName = "numberInTwentySequence.txt";

NumberInTwentySequenceLayer::NumberInTwentySequenceLayer()
: BaseLayer()
, sequenceView(nullptr)
, isDanced(false) {

}

NumberInTwentySequenceLayer::~NumberInTwentySequenceLayer() {

}

void NumberInTwentySequenceLayer::onEnter() {
    BaseLayer::onEnter();
}

void NumberInTwentySequenceLayer::onExit() {
    
    BaseLayer::onExit();
}

bool NumberInTwentySequenceLayer::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    sequenceView = SequenceView::create(12, 9);
    sequenceView->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    sequenceView->setPosition(Vec2(60, V_HEIGHT / 2));
    this->addChild(sequenceView);
    
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
        
        if(sequenceView->checkCanDance()) {
            isDanced = true;
            sequenceView->dance();
        }
    });
    this->addChild(danceBtn);
    
    Button* restoreBtn = Button::create();
    restoreBtn->setTitleFontSize(24);
    restoreBtn->setTitleColor(Color3B::BLACK);
    restoreBtn->setTitleText("恢復");
    restoreBtn->setPosition(Vec2(V_WIDTH - 100, 200));
    restoreBtn->addClickEventListener([this, restoreBtn](Ref* pSender){
        string result = FileUtils::getInstance()->getStringFromFile(FileUtils::getInstance()->getWritablePath() + "/" + SaveFileName);
        rapidjson::Document doc;
        doc.Parse(result.c_str());
        fromJson(doc);
        
        restoreBtn->setEnabled(false);
    });
    this->addChild(restoreBtn);
    
    return true;
}

void NumberInTwentySequenceLayer::onBackHandle() {
    rapidjson::Document doc(rapidjson::kObjectType);
    toJson(doc);
    string result = getStringFromJson(doc);
    FileUtils::getInstance()->writeStringToFile(result, FileUtils::getInstance()->getWritablePath() + "/" + SaveFileName);
}

void NumberInTwentySequenceLayer::fromJson(const rapidjson::Value &json) {
    if(!json.IsObject()) {
        return;
    }
    
    if(json.HasMember("isDanced")) {
        isDanced = json["isDanced"].GetBool();
    }
    
    if(sequenceView) {
        sequenceView->fromJson(json);
    }
}

void NumberInTwentySequenceLayer::toJson(rapidjson::Document &json) {
    json.AddMember("isDanced", isDanced, json.GetAllocator());
    
    if(sequenceView) {
        sequenceView->toJson(json);
    }
}
