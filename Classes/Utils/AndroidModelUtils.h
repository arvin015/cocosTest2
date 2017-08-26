//
//  AndroidModelUtils.h
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#ifndef AndroidModelUtils_h
#define AndroidModelUtils_h

#include <stdio.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"

#define JAVAVM cocos2d::JniHelper::getJavaVM()
#endif

class AndroidModelUtils {

public:

    AndroidModelUtils();
    ~AndroidModelUtils();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    static std::vector<std::string> getAndroidModel();
#endif
};

#endif /* AndroidModelUtils_h */
