//
//  NumberInTwentySequence.cpp
//  CocosTest2
//
//  Created by arvin on 2017/9/27.
//
//

#include "NumberInTwentySequence.h"
#include "SequenceView.h"

NumberInTwentySequenceLayer::NumberInTwentySequenceLayer()
: BaseLayer()
, sequenceView(nullptr) {

}

NumberInTwentySequenceLayer::~NumberInTwentySequenceLayer() {

}

void NumberInTwentySequenceLayer::onEnter() {
    BaseLayer::onEnter();
}

void NumberInTwentySequenceLayer::onExit() {
    
    BaseLayer::onExit();
}

bool NumberInTwentySequenceLayer::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    return true;
}
