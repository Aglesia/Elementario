LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../include

# Add your application source files here...
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../src/Affichage.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/Bouton.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/Controller.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/Manette.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/Touche.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/ToucheJeu.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/main.cpp

LOCAL_STATIC_LIBRARIES := SDL2_static

include $(BUILD_SHARED_LIBRARY)
$(call import-module,SDL)LOCAL_PATH := $(call my-dir)
