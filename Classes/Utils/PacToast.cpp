/*
 * PacToast.cpp
 *
 *  Created on: 2016年8月12日
 *      Author: arvin.li
 */

#include "PacToast.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const int TOAST_TAG = 321;

PacToast::PacToast() {
	// TODO Auto-generated constructor stub

}

PacToast::~PacToast() {
	// TODO Auto-generated destructor stub
}

void PacToast::makeText(const string &msg) {
    makeText(msg, 1.5);
}

void PacToast::makeText(const string &msg, float time) {
    Node* node = Director::getInstance()->getRunningScene();
    makeText(node, msg, time);
}

void PacToast::makeText(Node *node, const string &msg, float time, bool autoDismiss) {

    dismissToast();
    
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Label* pLabel = Label::createWithSystemFont(msg.c_str(), "Arial", 24);
	pLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	pLabel->setColor(Color3B::WHITE);

	Layout* bg = Layout::create();
	bg->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
	bg->setBackGroundColor(Color3B::BLACK);
	bg->setBackGroundColorOpacity(150);
	bg->setContentSize(pLabel->getContentSize() + Size(20, 15));
	bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	bg->setPosition(Vec2(visibleSize.width / 2, bg->getContentSize().height * 2));
	bg->addChild(pLabel);
    bg->setTag(TOAST_TAG);

	pLabel->setPosition(bg->getContentSize() / 2);

//	node->addChild(bg);
    Director::getInstance()->getRunningScene()->addChild(bg);

    if(autoDismiss) { //自动消失
        bg->runAction(Sequence::create(FadeIn::create(0.2f), DelayTime::create(time), FadeOut::create(0.2f),
                                       CallFuncN::create([bg](Ref* pSender){
            bg->removeFromParentAndCleanup(true);
        }), NULL));
    }
}

void PacToast::makeTextNoDismiss(const std::string &msg) {
    makeText(nullptr, msg, 2, false);
}

/**
 * 隐藏Toast
 */
void PacToast::dismissToast() {
    Director::getInstance()->getRunningScene()->removeChildByTag(TOAST_TAG);
}
