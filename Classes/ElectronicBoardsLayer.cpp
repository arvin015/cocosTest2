//
//  ElectronicBoardsLayer.cpp
//  CocosTest2
//
//  Created by arvin on 2017/10/12.
//
//

#include "ElectronicBoardsLayer.h"
#include "ElectronicBoards.h"

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

    electronicBoards = ElectronicBoards::create();
    electronicBoards->setBoard(6, 6);
    this->addChild(electronicBoards, 2);

    return true;
}
