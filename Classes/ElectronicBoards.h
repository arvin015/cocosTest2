//
//  ElectronicBoards.h
//  CocosTest2
//
//  Created by arvin on 2017/10/12.
//
//

#ifndef ElectronicBoards_h
#define ElectronicBoards_h

#include <stdio.h>
#include "cocos2d.h"

class ElectronicBoards : public cocos2d::Layer {

public:
    
    ElectronicBoards();
    ~ElectronicBoards();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(ElectronicBoards);
    
public:
    
    /**
     * 设置钉板基本数据
     */
    void initBoard(float boardWidth, float boardHeight, float padding);
    
    /**
     * 设置钉板
     */
    void setBoard(int row, int col);
    
private:
    
    /**
     * 创建钉子
     */
    cocos2d::Sprite* createNagSprite(const std::string &imageName, const cocos2d::Vec2 &position);
    
private:

    float boardWidth; //钉板宽
    float boardHeight; //钉板高
    float padding; //边距
    float startX; //开始X
    float startY; //开始Y
    float marginX; //纵间距
    float marginY; //横间距
    
    std::vector<float> colYList; //行Y值集合
    std::vector<float> rowXList; //列X值集合
};

#endif /* ElectronicBoards_h */
