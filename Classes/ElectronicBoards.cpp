//
//  ElectronicBoards.cpp
//  CocosTest2
//
//  Created by arvin on 2017/10/12.
//
//

#include "ElectronicBoards.h"

USING_NS_CC;
using namespace std;

const float START_X = 50;
const float START_Y = 25;

ElectronicBoards::ElectronicBoards() {
    boardWidth = 640;
    boardHeight = 640;
    padding = 40;
    startX = START_X + padding;
    startY = START_X + padding;
}

ElectronicBoards::~ElectronicBoards() {

}

void ElectronicBoards::onEnter() {
    Layer::onEnter();
}

void ElectronicBoards::onExit() {

    Layer::onExit();
}

bool ElectronicBoards::init() {
    if(Layer::init()) {
        return false;
    }
    
    return true;
}

void ElectronicBoards::initBoard(float boardWidth, float boardHeight, float padding) {
    this->boardWidth = boardWidth;
    this->boardHeight = boardHeight;
    this->padding = padding;
    
    startX = START_X + padding;
    startY = START_X + padding;
}

void ElectronicBoards::setBoard(int row, int col) {
    
    colYList.clear();
    rowXList.clear();
    
    marginX = (boardWidth - padding * 2) / (col - 1);
    marginY = (boardHeight - padding * 2) / (row - 1);
    
    for(int i = 0; i < row; i++) {
        colYList.push_back(startY + i * marginY);
    }
    for(int j = 0; j < col; j++) {
        rowXList.push_back(startX + j * marginX);
    }
    
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; i++) {
            
        }
    }
}

Sprite* ElectronicBoards::createNagSprite(const string &imageName, const Vec2 &position) {
    return nullptr;
}
