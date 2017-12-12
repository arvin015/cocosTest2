//
//  AddAndSubtractLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/5.
//

#include "AddAndSubtractLayer.h"
#include "AddAndSubtractNode.h"
#include "PacToast.h"
#include "CommonUtils.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const string SaveFileName = "AddAndSubtract.txt";

AddAndSubtractLayer::AddAndSubtractLayer()
: addAndSubtractNode(nullptr)
, isDanced(false) {

}

AddAndSubtractLayer::~AddAndSubtractLayer() {

}

void AddAndSubtractLayer::onEnter() {
    BaseLayer::onEnter();
}

void AddAndSubtractLayer::onExit() {
    BaseLayer::onExit();
}

bool AddAndSubtractLayer::init() {
    if (!BaseLayer::init()) {
        return false;
    }

    addAndSubtractNode = AddAndSubtractNode::create();
    addAndSubtractNode->setContentSize(Size(1024 - 280, 80));
    addAndSubtractNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    addAndSubtractNode->setPosition(Vec2(80, 768 / 2));
    addAndSubtractNode->setValueRange(1, 18);
    this->addChild(addAndSubtractNode);

    Button* danceBtn = Button::create();
    danceBtn->setPosition(Vec2(100, 100));
    danceBtn->setTitleText("舞动");
    danceBtn->setTitleFontSize(22);
    danceBtn->setTitleColor(Color3B::BLACK);
    danceBtn->addClickEventListener([this](Ref* pSender) {
        if (isDanced) {
            PacToast::makeText("該對象已舞動");
            return;
        }
        if (addAndSubtractNode->checkCanDance()) {
            isDanced = true;
            addAndSubtractNode->dance();
            restoreBtn->setVisible(false);

        }
    });
    this->addChild(danceBtn);

    restoreBtn = Button::create();
    restoreBtn->setTitleFontSize(22);
    restoreBtn->setTitleColor(Color3B::BLACK);
    restoreBtn->setTitleText("恢復");
    restoreBtn->setPosition(Vec2(danceBtn->getPositionX() + danceBtn->getContentSize().width + 50, danceBtn->getPositionY()));
    restoreBtn->addClickEventListener([this](Ref* pSender){
        string result = FileUtils::getInstance()->getStringFromFile(FileUtils::getInstance()->getWritablePath() + "/" + SaveFileName);
        rapidjson::Document doc;
        doc.Parse(result.c_str());
        fromJson(doc);

        restoreBtn->setVisible(false);
    });
    this->addChild(restoreBtn);

    return true;
}

void AddAndSubtractLayer::onBackHandle() {
    rapidjson::Document doc(rapidjson::kObjectType);
    toJson(doc);
    string result = getStringFromJson(doc);
    FileUtils::getInstance()->writeStringToFile(result, FileUtils::getInstance()->getWritablePath() + "/" + SaveFileName);
}

void AddAndSubtractLayer::fromJson(const rapidjson::Value &json) {
    
    if (!json.IsObject()) return;
    isDanced = json.HasMember("isDanced") ? json["isDanced"].GetBool() : false;

    if (addAndSubtractNode) {
        addAndSubtractNode->fromJson(json);
    }
}

void AddAndSubtractLayer::toJson(rapidjson::Document &json) {
    json.SetObject();

    json.AddMember("isDanced", isDanced, json.GetAllocator());

    if (addAndSubtractNode) {
        addAndSubtractNode->toJson(json);
    }
}
