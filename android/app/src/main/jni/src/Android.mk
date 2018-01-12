LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2
SDL_TTF_PATH := ../SDL2_ttf
SDL_IMAGE_PATH := ../SDL2_image

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../../include

# Add your application source files here...
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../src/Affichage.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/Bouton.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/Controller.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/Manette.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/Touche.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/ToucheJeu.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/Menu.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/BundleTouche.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/LiaisonTouche.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../../src/main.cpp

LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_SHARED_LIBRARIES += SDL2_ttf
LOCAL_SHARED_LIBRARIES += SDL2_image
LOCAL_CPPFLAGS  += -std=c++11

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
