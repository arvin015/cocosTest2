//
//  DrawViewTest.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#include "DrawViewTest.h"
#include "UIDrawView.h"
#include "ui/UICheckBox.h"

USING_NS_CC;
using namespace ui;
using namespace std;

DrawViewTest::DrawViewTest() {

}

DrawViewTest::~DrawViewTest() {

}

bool DrawViewTest::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    UIDrawView* drawView = UIDrawView::create();
    this->addChild(drawView);
    
    CheckBox* drawTypeBtn = CheckBox::create("CloseNormal.png", "CloseSelected.png");
    drawTypeBtn->setContentSize(Size(60, 40));
    drawTypeBtn->setPosition(Vec2(50, 50));
    drawTypeBtn->addEventListener([drawView](Ref* pSender, CheckBox::EventType eventType){
        if(eventType == CheckBox::EventType::SELECTED) {
            drawView->setDrawType(UIDrawView::CLEAR);
        } else {
            drawView->setDrawType(UIDrawView::DRAW);
        }
    });
    this->addChild(drawTypeBtn);
    
    return true;
}
