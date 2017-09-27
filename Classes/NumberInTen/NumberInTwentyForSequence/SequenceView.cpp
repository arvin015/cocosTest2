//
//  SequenceView.cpp
//  CocosTest2
//
//  Created by arvin on 2017/9/27.
//
//

#include "SequenceView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

SequenceView::SequenceView() {

}

SequenceView::~SequenceView() {

}

void SequenceView::onEnter() {
    Layout::onEnter();
}

void SequenceView::onExit() {

    Layout::onExit();
}

bool SequenceView::init() {
    if(!Layout::init()) {
        return false;
    }
    
    return true;
}
