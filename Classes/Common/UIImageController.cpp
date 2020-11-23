//
//  UIImageController.cpp
//  edxschool
//
//  Created by arvin on 2020/6/10.
//

#include "UIImageController.h"

USING_NS_CC;
using namespace ui;
using namespace std;
using namespace cocos2d::network;

ImageController::ImageController() : imageView(nullptr) {
    
}

ImageController::~ImageController() {
    
}

void ImageController::onEnter() {
    base::onEnter();
}

void ImageController::onExit() {
    base::onExit();
}

void ImageController::cleanup() {
    imageView = nullptr;
    base::cleanup();
}

ImageController* ImageController::create(const Size &size, const string &localDir, const string &defaultImage) {
    auto ref = new ImageController();
    if (ref && ref->init(size, localDir, defaultImage)) {
        ref->autorelease();
        return ref;
    }
    CC_SAFE_DELETE(ref);
    return nullptr;
}

bool ImageController::init(const Size &size, const string &localDir, const string &defaultImage) {
    if (!base::init()) {
        return false;
    }
    
    this->defaultImage = defaultImage;
    savePath = FileUtils::getInstance()->getWritablePath() + localDir;
    if (!FileUtils::getInstance()->isDirectoryExist(savePath)) {
        FileUtils::getInstance()->createDirectory(savePath);
    }
    
    setContentSize(size);
    
    imageView = ImageView::create(defaultImage);
    imageView->setPosition(Vec2(size / 2));
    addChild(imageView);
    
    return true;
}

void ImageController::setContentSize(const cocos2d::Size &size) {
    base::setContentSize(size);
}

void ImageController::setAnchorPoint(const cocos2d::Vec2 &anchorPoint) {
    setIgnoreAnchorPointForPosition(false);
    base::setAnchorPoint(anchorPoint);
}

void ImageController::load(const string &blobPath) {
    if (blobPath.empty()) {
        setImageFile(defaultImage);
        return;
    }
    
    fileName = blobPath.substr(blobPath.find_last_of('/') + 1, blobPath.length());
    
    string file = savePath + fileName;
    
    if (FileUtils::getInstance()->isFileExist(file)) {
        setImageFile(file);
    } else {
        loadFromNetwork(blobPath);
    }
}

void ImageController::setImageFile(const string &imageFile) {
    if (imageView && imageView->getParent()) {
        imageView->loadTexture(imageFile);
        adapterImageView();
    }
}

void ImageController::setBgColor(const Color3B &bgColor) {
    setBackGroundColorType(BackGroundColorType::SOLID);
    setBackGroundColor(bgColor);
}

void ImageController::setBgImage(const string &bgImage) {
    setBackGroundImage(bgImage);
}

void ImageController::loadFromNetwork(const std::string &url) {
    auto request = new HttpRequest();
    request->setUrl(encodeUrl(url));
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(ImageController::onImageLoadedCallback, this));
    request->setTag(url);
    HttpClient::getInstance()->send(request);
    request->release();
}

void ImageController::onImageLoadedCallback(HttpClient *sender, HttpResponse *response) {
    
    if (!response) {
        log("request network error: response is null!");
        return;
    }
    
    auto buffer = response->getResponseData();
    auto url = response->getHttpRequest()->getTag();
 
    if (!response->isSucceed()) {
        log("request network error: response code = %ld, info = %s", response->getResponseCode(), response->getErrorBuffer());
        return;
    }
    
    if (1000 > buffer->size()) {// 認為是圖片不存在
        log("request network error: image is not available or is to small! (size < 1000) url = %s", url);
        return ;
    }
    
    Data data;
    data.copy((const unsigned char*)buffer->data(), buffer->size());
    if ((savePath + fileName).empty()) return;
    
    bool result = FileUtils::getInstance()->writeDataToFile(data, savePath + fileName);
    if (result) {
        setImageFile(savePath + fileName);
    }
}

void ImageController::adapterImageView() {
    if (imageView == nullptr) return;
    
    float scale = 1;
    int maxWidth = getContentSize().width;
    int maxHeight = getContentSize().height;
    int imageWidth = imageView->getContentSize().width;
    int imageHeight = imageView->getContentSize().height;
    if (imageWidth >= imageHeight && imageWidth > maxWidth) {
        scale = maxWidth * 1.0f / imageWidth;
    } else if (imageHeight >= imageWidth && imageHeight > maxHeight) {
        scale = maxHeight * 1.0f / imageHeight;
    }
    imageView->setScale(scale);
}

string ImageController::encodeUrl(const string &url) {
    ssize_t pos = url.find(" ");
    if (string::npos == pos) {
        return url;
    }
    
    string result;
    for (ssize_t i = 0; i < url.length(); ++i) {
        if (' ' == url[i]) {
            result += "%20";
        } else {
            result += url[i];
        }
    }
    return result;
}
