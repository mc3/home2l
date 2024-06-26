# This file is part of the Home2L project.
#
# (C) 2015-2021 Gundolf Kiefer
#
# Home2L is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Home2L is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Home2L. If not, see <https://www.gnu.org/licenses/>.


LOCAL_PATH := $(call my-dir)



##### SDL2 #####

include $(CLEAR_VARS)

LOCAL_MODULE := SDL2_static

LOCAL_SRC_FILES := $(LOCAL_PATH)/usr/android/lib/libSDL2.a

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/usr/android/include/
LOCAL_EXPORT_LDLIBS := -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit -ldl -lGLESv1_CM -lGLESv2 -llog -landroid

include $(PREBUILT_STATIC_LIBRARY)


#~ # HID API (would be required by SDL2 >= 2.0.9) ...
#~ include $(CLEAR_VARS)

#~ LOCAL_MODULE := hidapi_static
#~ LOCAL_SRC_FILES += $(LOCAL_PATH)/usr/android/lib/libhidapi.a

#~ include $(PREBUILT_STATIC_LIBRARY)



##### SDL2_ttf (+ freetype) #####

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_ttf_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/usr/android/lib/libSDL2_ttf.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := freetype
LOCAL_SRC_FILES := $(LOCAL_PATH)/usr/android/lib/libfreetype.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := harfbuzz
LOCAL_SRC_FILES := $(LOCAL_PATH)/usr/android/lib/libharfbuzz.a
include $(PREBUILT_STATIC_LIBRARY)



##### Import 'cpufeatures' #####

# Required for SDL2 2.28.5 [2024-01-04]
# Also, "cpufeatures" must be added to LOCAL_STATIC_LIBRARIES in the main Android.mk file.
# See: http://api.suwish.com/android/ndk/guides/cpu-features.html

$(call import-module,android/cpufeatures)
