//
//  UIImageController.h
//  edxschool
//
//  Created by arvin on 2020/6/10.
//

#ifndef UIImageController_h
#define UIImageController_h

#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"
#include "network/HttpClient.h"

class ImageController : public cocos2d::ui::Layout {
    
    typedef cocos2d::ui::Layout base;
    typedef ImageController thisclass;
    
public:
    
    ImageController();
    virtual ~ImageController();
    
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void cleanup() override;
    
    static ImageController* create(const cocos2d::Size &size, const std::string &localDir, const std::string &defaultImage);
    bool init(const cocos2d::Size &size, const std::string &localDir, const std::string &defaultImage);
    
    virtual void setAnchorPoint(const cocos2d::Vec2 &anchorPoint) override;
    
    void load(const std::string &blobPath);
    
    void setImageFile(const std::string &imageFile);
    
    void setBgColor(const cocos2d::Color3B &bgColor);
    
    void setBgImage(const std::string &bgImage);
    
private:
    
    virtual void setContentSize(const cocos2d::Size &size) override;
    
    void loadFromNetwork(const std::string &url);
    
    void onImageLoadedCallback(cocos2d::network::HttpClient* sender, cocos2d::network::HttpResponse* response);
    
    void adapterImageView();
    
    std::string encodeUrl(const std::string &url);
    
private:
    
    std::string savePath;
    std::string fileName;
    std::string defaultImage;
    
    cocos2d::ui::ImageView* imageView;
};

#endif /* UIImageController_h */
