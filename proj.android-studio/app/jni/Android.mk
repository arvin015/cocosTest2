LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := hellocpp/main.cpp \
                    ../../../Classes/Main/AppDelegate.cpp \
                    ../../../Classes/Main/HelloWorldScene.cpp \
                    ../../../Classes/Main/NavigationController.cpp \
                    ../../../classes/Main/BaseLayer.cpp \
                    ../../../classes/Main/MainLayer.cpp \
                    ../../../classes/DrawViewTest.cpp \
                    ../../../Classes/UIDrawView.cpp \
                    ../../../Classes/Common/UICommonTableView.cpp \
                    ../../../Classes/UIDatasetTableView.cpp \
                    ../../../Classes/DatasetInfo.cpp \
                    ../../../Classes/Utils/AndroidModelUtils.cpp \
                    ../../../Classes/ActionTest.cpp \
                    ../../../Classes/ClippingTest.cpp \
                    ../../../Classes/TableViewTest.cpp

#FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/*.cpp)
#FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Main/*.cpp)
#FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Common/*.cpp)
#FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Utils/*.cpp)

#LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes/Main
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes/Common
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes/Utils

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
