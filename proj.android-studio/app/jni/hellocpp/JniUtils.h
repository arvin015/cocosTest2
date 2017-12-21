//
// Created by mac_arvin on 2017/12/21.
//

#ifndef PROJ_ANDROID_STUDIO_JNIUTILS_H
#define PROJ_ANDROID_STUDIO_JNIUTILS_H

#include "platform/android/jni/JniHelper.h"

class JniUtils {

};

extern "C" {
    JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_JniHelper_test(JNIEnv*, jobject, jstring);
}


#endif //PROJ_ANDROID_STUDIO_JNIUTILS_H
