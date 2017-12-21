//
// Created by mac_arvin on 2017/12/21.
//

#include "JniUtils.h"

JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_JniHelper_test(JNIEnv* env, jobject obj, jstring str) {

    std::string value = cocos2d::JniHelper::jstring2string(str);

    value = value + " world";

    jstring rs = env->NewStringUTF(value.c_str());

    return rs;
}
