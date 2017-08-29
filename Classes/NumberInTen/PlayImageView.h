//
// Created by Administrator on 2017/8/27.
//

#ifndef PROJ_ANDROID_STUDIO_PlayImageView_H
#define PROJ_ANDROID_STUDIO_PlayImageView_H

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UIImageView.h"

class PlayImageView : public cocos2d::ui::ImageView {

public:

    PlayImageView();
    ~PlayImageView();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();
    bool init(const std::string& imageFileName, TextureResType texType = TextureResType::LOCAL);

    CREATE_FUNC(PlayImageView);

    static PlayImageView* create(const std::string &normalFilename, const std::string &disableFilename);
    
    /**
     * 重载
     */
    virtual void setEnabled(bool enabled);

private:
    std::string normalFilename;
    std::string disableFilename;
};


#endif //PROJ_ANDROID_STUDIO_DRAGSPRITE_H
