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

class BandView;

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
    
    /**
     * 加入橡皮筋
     */
    void addBand(const cocos2d::Color4F &color);
    
private:
    
    /**
     * 创建钉子
     */
    cocos2d::Sprite* createSnagSprite(const std::string &imageName, const cocos2d::Vec2 &position);
    
private:

    float boardWidth; //钉板宽
    float boardHeight; //钉板高
    float padding; //边距
    float startX; //开始X
    float startY; //开始Y
    float marginX; //纵间距
    float marginY; //横间距
    int totalRow; //总行数
    int totalCol; //总列数
    
    std::map<int, float> rowYMap; //行Y值Map集合
    std::map<int, float> colXMap; //列X值Map集合
    cocos2d::Vector<cocos2d::Sprite*> snapSpriteList;
    
    BandView* curBandView; //当前橡皮筋
};

#endif /* ElectronicBoards_h */
