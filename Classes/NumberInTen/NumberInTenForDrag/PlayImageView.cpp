//
// Created by Administrator on 2017/8/27.
//

#include "PlayImageView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

PlayImageView::PlayImageView() {

}

PlayImageView::~PlayImageView() {

}

void PlayImageView::onEnter() {
    ImageView::onEnter();
}
void PlayImageView::onExit() {
    ImageView::onExit();
}

bool PlayImageView::init() {
    if(!ImageView::init()) {
        return false;
    }

    return true;
}

bool PlayImageView::init(const std::string &imageFileName, TextureResType texType)
{
    bool bRet = true;
    do
    {
        if (!init())
        {
            bRet = false;
            break;
        }

        this->loadTexture(imageFileName, texType);
    } while (0);
    return bRet;
}

PlayImageView* PlayImageView::create(const string &normalFilename, const string &disableFilename) {
    PlayImageView *widget = new (std::nothrow) PlayImageView;
    if (widget && widget->init(normalFilename))
    {
        widget->normalFilename = normalFilename;
        widget->disableFilename = disableFilename;
        
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

void PlayImageView::setEnabled(bool enabled) {
    ImageView::setEnabled(enabled);
    
    if(enabled) {
        loadTexture(normalFilename);
    } else {
        loadTexture(disableFilename);
    }
}
