LOCAL_PATH:= $(call my-dir)

# ------------------------------------------------------------------
# Static library
# ------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libgammaengine

ifneq (,$(filter $(TARGET_ARCH), x86_64 arm64 arm64-v8 mips64))
	ARCH = 64
else
	ARCH = 32
endif

SRC := $(wildcard ../src/*.cpp)
SRC += $(wildcard ../backends/opengles20/src/*.cpp)
SRC += $(wildcard ../src/android/*.cpp)
SRC := $(filter-out ../src/Vector.cpp, $(SRC))
SRC := $(filter-out ../src/m.cpp, $(SRC))

$(info $(SRC))

LOCAL_SRC_FILES = $(addprefix ../, $(SRC))

LOCAL_CPPFLAGS := -Wall -Wno-unused -std=gnu++0x -std=c++11 -fPIC -I../ -I../include -I../include/android -I../backends/opengles20/include -DGE_LIB -DGE_ANDROID -DGE_STATIC_BACKEND=opengles20

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

# LOCAL_LDLIBS    := -lfreetype2-static -ljpeg9 -lpng -lz -lm -llog -landroid -lEGL -lGLESv2
LOCAL_WHOLE_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_STATIC_LIBRARY)
# include $(BUILD_SHARED_LIBRARY)

$(call import-module, android/native_app_glue)
