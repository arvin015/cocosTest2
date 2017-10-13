//
//  ElectronicBoardsLayer.cpp
//  CocosTest2
//
//  Created by arvin on 2017/10/12.
//
//

#include "ElectronicBoardsLayer.h"
#include "ElectronicBoards.h"

USING_NS_CC;
using namespace ui;
using namespace std;

ElectronicBoardsLayer::ElectronicBoardsLayer() {
    electronicBoards = nullptr;
}

ElectronicBoardsLayer::~ElectronicBoardsLayer() {

}

void ElectronicBoardsLayer::onEnter() {
    BaseLayer::onEnter();
}

void ElectronicBoardsLayer::onExit() {

    BaseLayer::onExit();
}

bool ElectronicBoardsLayer::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    Button* bandBtn = Button::create();
    bandBtn->setTitleText("橡皮筋");
    bandBtn->setTitleColor(Color3B::BLUE);
    bandBtn->setPosition(Vec2(V_WIDTH - 200, V_HEIGHT / 2));
    bandBtn->setTitleFontSize(24);
    bandBtn->addClickEventListener([this](Ref* pSender){
        electronicBoards->addBand(Color4F::BLUE);
    });
    this->addChild(bandBtn);

    electronicBoards = ElectronicBoards::create();
    electronicBoards->setBoard(6, 6);
    this->addChild(electronicBoards);

    return true;
}
