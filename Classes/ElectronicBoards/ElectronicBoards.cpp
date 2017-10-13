//
//  ElectronicBoards.cpp
//  CocosTest2
//
//  Created by arvin on 2017/10/12.
//
//

#include "ElectronicBoards.h"
#include "BandView.h"

USING_NS_CC;
using namespace std;

const float START_X = 50;
const float START_Y = 25;

ElectronicBoards::ElectronicBoards() {
    boardWidth = 640;
    boardHeight = 640;
    padding = 40;
    startX = START_X + padding;
    startY = START_Y + padding;
    curBandView = nullptr;
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
    if(!Layer::init()) {
        return false;
    }
    
    return true;
}

void ElectronicBoards::initBoard(float boardWidth, float boardHeight, float padding) {
    this->boardWidth = boardWidth;
    this->boardHeight = boardHeight;
    this->padding = padding;
    
    startX = START_X + padding;
    startY = START_Y + padding;
}

void ElectronicBoards::setBoard(int row, int col) {
    
    this->totalRow = row;
    this->totalCol = col;
    
    colXMap.clear();
    rowYMap.clear();
    
    marginX = (boardWidth - padding * 2) / (col - 1);
    marginY = (boardHeight - padding * 2) / (row - 1);
    
    for(int i = 0; i < row; i++) {
        rowYMap.insert(make_pair(i + 1, startY + i * marginY));
    }
    for(int j = 0; j < col; j++) {
        colXMap.insert(make_pair(j + 1, startX + j * marginX));
    }
    
    for(int i = 0; i < row; i++) {

        float y = rowYMap.at(i + 1);

        for(int j = 0; j < col; j++) {
            float x = colXMap.at(j + 1);
            Sprite* snapSprite = createSnagSprite("snap.png", Vec2(x, y));
            snapSprite->setName(StringUtils::format("%d-%d", row, col));
            this->addChild(snapSprite, 999);
        }
    }
}

void ElectronicBoards::addBand(const Color4F &color) {
    Vec2 startPoint = Vec2(colXMap.at(totalCol / 2), rowYMap.at(totalRow / 2));
    Vec2 endPoint = Vec2(colXMap.at(totalCol / 2 + 1), rowYMap.at(totalRow / 2));
    
    curBandView = BandView::create();
    curBandView->initData(startPoint, endPoint, color);
    curBandView->setTag(getChildrenCount() + 1);
    curBandView->setOnBandTouchEndedListener([this](BandView* bandView, float x, float y){
        float rx = x - START_X - padding;
        float ry = y - START_Y - padding;
        
        //越界处理
        int minRow = ry / marginY + 1;
        if(minRow < 1) {
            minRow = 1;
        } else if(minRow > totalRow) {
            minRow = totalRow;
        }
        int maxRow = minRow == totalRow ? minRow : minRow + 1;
        
        //越界处理
        int minCol = rx / marginX + 1;
        if(minCol < 1) {
            minCol = 1;
        } else if(minCol > totalCol) {
            minCol = totalCol;
        }
        int maxCol = minCol == totalCol ? minCol : minCol + 1;
    
        int rRow = abs(y - rowYMap.at(minRow)) > abs(y - rowYMap.at(maxRow)) ? maxRow : minRow;
        int rCol = abs(x - colXMap.at(minCol)) > abs(x - colXMap.at(maxCol)) ? maxCol : minCol;
        
        bandView->update(Vec2(colXMap.at(rCol), rowYMap.at(rRow)));
    });
    this->addChild(curBandView);
}

Sprite* ElectronicBoards::createSnagSprite(const string &imageName, const Vec2 &position) {
    Sprite* sprite = Sprite::create(imageName);
    sprite->setPosition(position);
    return sprite;
}
