LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

#LOCAL_SRC_FILES := hellocpp/main.cpp \
#                    ../../../Classes/Main/AppDelegate.cpp \
#                    ../../../Classes/Main/HelloWorldScene.cpp \
#                    ../../../Classes/Main/NavigationController.cpp \
#                    ../../../classes/Main/BaseLayer.cpp \
#                    ../../../classes/Main/MainLayer.cpp \
#                    ../../../classes/DrawViewTest.cpp \
#                    ../../../Classes/UIDrawView.cpp \
#                    ../../../Classes/Common/UICommonTableView.cpp \
#                    ../../../Classes/UIDatasetTableView.cpp \
#                    ../../../Classes/DatasetInfo.cpp \
#                    ../../../Classes/Utils/AndroidModelUtils.cpp \
#                    ../../../Classes/ActionTest.cpp \
#                    ../../../Classes/ClippingTest.cpp \
#                    ../../../Classes/TableViewTest.cpp

FILE_LIST := $(wildcard $(LOCAL_PATH)/hellocpp/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Common/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Utils/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/NumberInTen/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/NumberInTen/NumberInTenForDrag/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/NumberInTen/NumberInTenForFly/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/NumberInTen/NumberInTwentyForSequence/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/ElectronicBoards/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/AddAndSubtract/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Cutting/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Test3D/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Test3D/cc2d3/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Test3D/cc3d3/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/FoldPaper/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/FractionBar/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/VolumesOfCubes/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/VolumeCutfill/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/VectorTestLayer/*.cpp)

LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/hellocpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/Common
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/Utils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/NumberInTen
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/NumberInTen/NumberInTenForDrag
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/NumberInTen/NumberInTenForFly
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/NumberInTen/NumberInTwentyForSequence
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/ElectronicBoards
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/AddAndSubtract
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/Cutting
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/Test3D
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/Test3D/cc2d3
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/Test3D/cc3d3
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/FoldPaper
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/FractionBar
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/VolumesOfCubes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/VolumeCutfill
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/VectorTestLayer

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
