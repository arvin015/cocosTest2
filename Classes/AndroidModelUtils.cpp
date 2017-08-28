//
//  AndroidModelUtils.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#include "AndroidModelUtils.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C"
{
    static std::string jni_GetStaticField_string(const char* className, const char* fieldName)
    {
        std::string ret;
        
        do
        {
            JNIEnv *pEnv = 0;
            
            if (JAVAVM->GetEnv((void**)&pEnv, JNI_VERSION_1_4) != JNI_OK)
            {
                break;
            }
            
            if (JAVAVM->AttachCurrentThread(&pEnv, 0) < 0)
            {
                break;
            }
            
            jclass classID = pEnv->FindClass(className);
            if (!classID)
            {
                break;
            }
            
            jfieldID fieldID = pEnv->GetStaticFieldID(classID, fieldName, "Ljava/lang/String;");
            if(!fieldID)
            {
                break;
            }
            
            jstring jstr = (jstring)pEnv->GetStaticObjectField(classID, fieldID);
            
            const char* chars = pEnv->GetStringUTFChars(jstr, NULL);
            ret = chars;
            pEnv->ReleaseStringUTFChars(jstr, chars);
        } while (0);
        
        return ret;
    }
}

std::vector<std::string> AndroidModelUtils::getAndroidModel()
{
#define CLASS_NAME "android/os/Build"
    std::string BRAND = jni_GetStaticField_string(CLASS_NAME, "BRAND");
    std::string DEVICE = jni_GetStaticField_string(CLASS_NAME, "DEVICE");
    std::string MODEL = jni_GetStaticField_string(CLASS_NAME, "MODEL");

    std::vector<std::string> modelVector;
    modelVector.push_back(BRAND);
    modelVector.push_back(DEVICE);
    modelVector.push_back(MODEL);

    return modelVector;
}

#endif
