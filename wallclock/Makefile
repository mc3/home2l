# This file is part of the Home2L project.
#
# (C) 2015-2024 Gundolf Kiefer
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





############################## Configuration ###################################


WITH_ANDROID ?= 1
	# Build Android APK (requires Android SDK and NDK to build)

# IP phone backend (if WITH_PHONE == 1) ...
#   Note: Presently, only 'pjsip' is tested and supported.
PHONE_LIB ?= pjsip

WITH_GSTREAMER ?= 1


# Location of the Android SDK and NDK (for the Android app only) ...
ANDROID_SDK ?= /opt/android/sdk
ANDROID_NDK ?= /opt/android/sdk/ndk/25.2.9519653
	# Note [2024-01-04]:
	#   NDK r25 is the last version supporting API level 19/20 (Android Kitkat),
	#   see https://github.com/android/ndk/wiki/Changelog-r26 for details.
	#   Since Kitkat was a popular release, and for home automation large number
	#   of older tablets may be still be in use, Home2L still supports API levels
	#   back to 19.





############################## Setup ###########################################


# Identify myself inside the Home2L source tree ...
#   (values may be overwritten by some custom build system)
HOME2L_SRC ?= ..
HOME2L_MOD ?= wallclock


# Setup...
include $(HOME2L_SRC)/Setup.mk
include $(HOME2L_SRC)/common/Env.mk
include $(HOME2L_SRC)/resources/Resources.mk


# Generic C/C++ flags...
#~ SANITIZE := -fsanitize=address   #-fsanitize=thread # 'thread' requires libtsan, which is only available for amd64 (Debian Jessie)
CFLAGS += $(SANITIZE)
LDFLAGS += -rdynamic $(SANITIZE)


# SDL2 ...
include ../external/sdl2/Debian.mk


# Apps ...
SRC += app_home.C

ifeq ($(WITH_PHONE),1)
	include ../external/$(PHONE_LIB)/Debian.mk
	SRC += app_phone.C ../common/phone.C ../common/phone-$(PHONE_LIB).C
endif

ifeq ($(WITH_CALENDAR),1)
	SRC += app_calendar.C
endif

ifeq ($(WITH_MUSIC),1)
	include ../external/mpdclient/Debian.mk
	SRC += ../common/streamer.C app_music.C
endif

ifeq ($(WITH_GSTREAMER),1)
	include ../external/gstreamer/Debian.mk
endif





############################## WallClock / Main part ###########################


CFLAGS += "-pg"


WALLCLOCK := home2l-wallclock
WALLCLOCK_BIN := $(DIR_OBJ)/$(WALLCLOCK)
SRC := $(SRC) ui_base.C ui_screen.C ui_widgets.C system.C apps.C alarmclock.C floorplan.C $(WALLCLOCK).C
OBJ := $(SRC:%.C=$(DIR_OBJ)/%.o)


$(WALLCLOCK_BIN): $(DEP_CONFIG) $(OBJ) icons.build
	@echo LD$(LD_SUFF) $(WALLCLOCK)
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)


all: $(WALLCLOCK_BIN)


run: $(WALLCLOCK_BIN)
	HOME2L_ROOT=$(abspath $(HOME2L_INSTALL)) $(WALLCLOCK_BIN)


# Automatic dependencies...
-include $(OBJ:%.o=%.d)


# Icons...
icons.build:
	@echo "Generating icons..."; \
	./generate-icons.sh





############################## Android #########################################

# Building the Android app is basically done in 3 stages, controlled by this
# Makefile (not Android/Gradle):
#
# 1. Preparation of the build directory in $(HOME2L_BUILD)
#    - copying 'android/' to $(HOME2L_BUILD)
#    - generating/copying all asset files into it
#
# 2. Building all native code
#    - This requires Android NDK only (no Android Studio or SDK )
#    - Last tested with: Android NDK r25
#    - Results are (only) 'libhome2l-wallclock.so' and optionally some other
#      external .so files (e.g. for GStreamer).
#    - Operates on directory 'android/jni' and 'android/', outputs put into
#      'android/lib' and 'android/obj'.
#
# 3. Building the APK
#    - This requires Android SDK (only) and Java.
#    - Last tested with: Android SDK Platform 34, OpenJDK 17
#    - Uses Gradle (Android default), but includes the native code (main part
#      of the app) as prebuilt libraries.


# Setup ...

APK_DIR := android
APK_BUILD := $(HOME2L_BUILD)/android
APK_BUILD_SRC := $(APK_BUILD)/app/src/main
APK_JOBS := -j8

APK_JAVA_SRC_FILES := Home2lService.java Home2lBootUpReceiver.java Home2lActivity.java Home2l.java

APK := $(APK_BUILD)/app/build/outputs/apk/debug/app-debug.apk
APK_JAVA_SRC := $(APK_JAVA_SRC_FILES:%=android/app/src/main/java/org/home2l/app/%)
APK_JAVA_CONFIG := $(APK_BUILD_SRC)/java/org/home2l/app/Home2lConfig.java

APK_NDK_SO := $(APK_BUILD)/libs/armeabi-v7a/libhome2l-wallclock.so
APK_NDK_FLAGS := -C $(APK_BUILD)/jni \
		DIR_SRC=$(abspath $(HOME2L_SRC))/$(HOME2L_MOD) \
		WITH_PHONE=$(WITH_PHONE) PHONE_LIB=$(PHONE_LIB) \
		WITH_MUSIC=$(WITH_MUSIC) WITH_GSTREAMER=$(WITH_GSTREAMER)


# Building...

.PHONY: print-sources print-config

print-sources:
	@echo $(APK_BUILD)/config.C $(SRC)

print-config:
	@echo -I$(APK_BUILD)


.PHONY: apk-ndk apk-javaconfig apk apk-ndk-clean apk-clean
apk-ndk: $(APK_NDK_SO)
apk-javaconfig: $(APK_JAVA_CONFIG)
apk: $(APK)


# Note: Target $(APK_BUILD) collects all assets, but its dependencies are incomplete.
#       On changes of the assets (icons, sounds, fonts, localization),
#       'make apk-clean' must be called manually before.
#
$(APK_BUILD): $(DEP_CONFIG) icons.build h2l-sysinfo.sh android-setadb.sh android-fixattrs.sh
	@echo "Creating/updating Android build dir $(APK_BUILD)..."
	@mkdir -p $(APK_BUILD)
	@cp -auL android/* $(APK_BUILD)
	@echo "sdk.dir=$(ANDROID_SDK)" > $(APK_BUILD)/local.properties
	@#
	@rm -fr $(APK_BUILD_SRC)/assets
	@mkdir -p $(APK_BUILD_SRC)/assets
	@#
	@mkdir -p $(APK_BUILD_SRC)/assets/share/icons
	@cp -a icons.build/*.bmp $(APK_BUILD_SRC)/assets/share/icons
	@mkdir -p $(APK_BUILD_SRC)/assets/share/fonts
	@cp -a fonts/*.ttf $(APK_BUILD_SRC)/assets/share/fonts
	@mkdir -p $(APK_BUILD_SRC)/assets/share/sounds
	@cp -a sounds/*.wav $(APK_BUILD_SRC)/assets/share/sounds
	@mkdir -p $(APK_BUILD_SRC)/assets/bin
	@cp -a h2l-sysinfo-android.sh $(APK_BUILD_SRC)/assets/bin/h2l-sysinfo.sh
	@cp -a android-setadb.sh $(APK_BUILD_SRC)/assets/bin/
	@cp -a android-fixattrs.sh $(APK_BUILD_SRC)/assets/bin/
	@cp -a ../external/remind/usr/android/bin/* $(APK_BUILD_SRC)/assets/bin/
	@cp -a ../external/gnu_patch/usr/android/bin/* $(APK_BUILD_SRC)/assets/bin/
	@#
	@$(MAKE) -C ../locales HOME2L_INSTALL=$(APK_BUILD_SRC)/assets install-indep
	@#
	@$(MAKE) -C ../showcase etc/floorplan.fpo
	@cp -aL ../showcase/etc $(APK_BUILD_SRC)/assets
	@cp -a ../showcase/var $(APK_BUILD_SRC)/assets
	@#
	@(cd $(APK_BUILD_SRC)/assets; find * -type f > FILES)
	@#
	@echo "Home2l "$(BUILD_VERSION)" "$(BUILD_DATE) > $(APK_BUILD_SRC)/assets/VERSION


$(APK_NDK_SO): $(SRC) $(APK_BUILD)
	@echo -e "\n### Building native library using Android NDK ...\n"
	@$(MAKE) ARCH=android config
	$(ANDROID_NDK)/ndk-build $(APK_JOBS) -s $(APK_NDK_FLAGS)


$(APK_JAVA_CONFIG): $(APK_NDK_SO)
	@echo "Creating 'Home2lConfig.java'..."
	@echo "// This file is auto-generated by the Home2L build system. DO NOT EDIT!" > $(APK_JAVA_CONFIG); \
	echo "" >> $(APK_JAVA_CONFIG); \
	echo "package org.home2l.app;" >> $(APK_JAVA_CONFIG); \
	echo "" >> $(APK_JAVA_CONFIG); \
	echo "public class Home2lConfig {" >> $(APK_JAVA_CONFIG); \
	echo "  public static final boolean withGstreamer = ( $(WITH_GSTREAMER) != 0 );" >> $(APK_JAVA_CONFIG); \
	echo "  public static final String[] nativeLibraries = {" >> $(APK_JAVA_CONFIG); \
	FILES="\"$(WALLCLOCK)\""; \
	SHLIBS=`$(ANDROID_NDK)/ndk-build -s $(APK_NDK_FLAGS) home2l_print_shlibs | grep -v ^make`; \
	echo "#### SHLIBS = '$$SHLIBS' ####" > /dev/null; \
	if [[ "$$SHLIBS" != "" ]]; then \
		for L in $$SHLIBS; do FILES="\"$$L\", $$FILES"; done; \
	fi; \
	echo "      $$FILES" >> $(APK_JAVA_CONFIG); \
	echo "    };" >> $(APK_JAVA_CONFIG); \
	echo "}" >> $(APK_JAVA_CONFIG)


$(APK): $(APK_NDK_SO) $(APK_JAVA_SRC) $(APK_JAVA_CONFIG)
	@echo -e "\n### Building APK using Gradle ...\n"
	@if [[ "$$PHONE_LIB" == "linphone" ]]; then cp -vau ../external/linphone/usr/android/lib/liblinphone.jar $(APK_BUILD_SRC)/libs; fi; \
	rm -fr $(APK_BUILD_SRC)/jniLibs && cp -a $(APK_BUILD)/libs $(APK_BUILD_SRC)/jniLibs; \
	cp -vauL android/* $(APK_BUILD)/; \
	$(APK_BUILD)/gradlew -p $(APK_BUILD) build


apk-ndk-clean:
	$(ANDROID_NDK)/ndk-build $(APK_NDK_FLAGS) clean


apk-clean:
	rm -fr $(APK_BUILD)


apk-stack: $(APK)
	home2l adb $(MACHINE) logcat | $(ANDROID_NDK)/ndk-stack -sym $(APK_BUILD)/obj/local/armeabi-v7a/





############################## Debugging #######################################

# Debugging on '001' or any other device...
#   This section is used by the developer as a scratch area for various debugging tasks.

ID001=$(shell grep 001 /opt/home2l/etc/androidb.conf | grep -v '^ *#' | tr -s ' ' '\t' | cut -f3)
HOST001=inf001wlan  # change this to debug another influence device


001-install: 001-install-apk 001-install-etc

001-install-etc:
	@HOME2L_ROOT=/data/data/org.home2l.app/files/home2l; \
	HOME2L_UID=`adb -s $(ID001) shell stat -t $$HOME2L_ROOT/VERSION | awk '{print $$5}'`; \
	#echo HOME2L_ROOT=$$HOME2L_ROOT HOME2L_UID=$$HOME2L_UID; \
	adb -s $(ID001) root; \
	adb -s $(ID001) shell rm -fr $$HOME2L_ROOT/etc; \
	adb -s $(ID001) push `realpath /opt/home2l/etc` $$HOME2L_ROOT/etc; \
	adb -s $(ID001) shell $$HOME2L_ROOT/bin/android-fixattrs.sh $$HOME2L_ROOT/etc $$HOME2L_UID; \
	adb -s $(ID001) unroot || true

#~ 	adb -s $(ID001) root; \
#~ 	adb -s $(ID001) shell su $$HOME2L_UID mkdir -p $$HOME2L_ROOT/.update; \
#~ 	adb -s $(ID001) shell su $$HOME2L_UID chmod 777 $$HOME2L_ROOT/.update; \
#~ 	adb -s $(ID001) shell rm -fr $$HOME2L_ROOT/.update/etc; \
#~ 	adb -s $(ID001) push `realpath /opt/home2l/etc` $$HOME2L_ROOT/.update; \
#~ 	adb -s $(ID001) shell su $$HOME2L_UID rm -fr $$HOME2L_ROOT/etc; \
#~ 	adb -s $(ID001) shell su $$HOME2L_UID cp -r $$HOME2L_ROOT/.update/etc $$HOME2L_ROOT/ | grep -v 'etc/gpio\.'; \
#~ 	adb -s $(ID001) shell rm -fr $$HOME2L_ROOT/.update/etc; \
#~ 	adb -s $(ID001) shell su $$HOME2L_UID rmdir $$HOME2L_ROOT/.update; \
#~ 	adb -s $(ID001) shell su $$HOME2L_UID chmod -R 700 $$HOME2L_ROOT/etc; \
#~ 	adb -s $(ID001) unroot || true

#~ 	HOME2L_UID=`adb -s $(ID001) shell su $$HOME2L_UID ls -l $$HOME2L_ROOT/VERSION | awk '{print $$3}'`; \
#~ 	adb -s $(ID001) shell su chown -R $$HOME2L_UID.$$HOME2L_UID $$HOME2L_ROOT/etc; \


#	home2l rollout -y $(HOST001) -b -e

001-install-apk:
	adb -s $(ID001) install -r $(APK)
	adb -s $(ID001) logcat -c

001-logcat:
	adb -s $(ID001) logcat -v time home2l:D SDL:V *:E *:W *:I

001-start:
	adb -s $(ID001) logcat -c
	adb -s $(ID001) shell am start org.home2l.app/org.home2l.app.Home2lActivity

001-stop:
	adb -s $(ID001) shell am force-stop org.home2l.app

001-restart: 001-stop 001-start

001-run: 001-restart 001-logcat

001-stack:
	adb -s $(ID001) logcat | $(ANDROID_NDK)/ndk-stack -sym $(APK_BUILD)/obj/local/armeabi-v7a/

001-forward:
	adb -s $(ID001) forward tcp:7020 tcp:1194


#~ Kill with stack trace: 2* "kill -6 <PID>" (6 = SIGABRT)
#~
#~ From https://groups.google.com/forum/#!topic/android-ndk/IZ4k0QC6zOQ:
#~ 		On Oct 20, 10:22 am, Bryan  Ashby <nuskoo...@gmail.com> wrote:
#~ 		> What I'm after is a way to force the application to crash (e.g. a
#~ 		> unhandled signal or such) that will then dump a stack trace to logcat.
#~ 		> Is this possible?
#~
#~ 		Hit the process with the same signal twice.  Signal 6 (SIGABRT) will
#~ 		work for this.
#~
#~ 		The first signal causes the signal handler in the process to wake up
#~ 		debuggerd, which attaches with ptrace.  The second signal causes a
#~ 		ptrace event that results in debuggerd logging the stacks.  (The log
#~ 		will only show the main thread; look in /data/tombstones for the
#~ 		rest.)



# Debugging in emulator (unmaintained)...

emu-start:
	LD_LIBRARY_PATH=/opt/android-sdk/tools/lib/ emulator-arm -gpu on @AVD_for_7_WSVGA_Tablet & \
		# important: 'gpu on', otherwise SDL cannot initialize

emu-pause:
	pkill -STOP emulator-arm

emu-stop:
	pkill adb

emu-install:
	adb connect localhost
	adb install -r $(APK)

emu-run:
	pkill -CONT emulator-arm
	adb connect localhost
	adb logcat -c
	adb install -r $(APK)
	adb logcat home2l:V SDL:V *:S





############################ Build system targets ##############################


clean: clean-build apk-clean


veryclean: clean
	rm -fr icons.build


ifeq ($(ARCH),amd64)   # presently, armhf and i386 are unsupported (not needed, not tested, may work anyway)


build-arch: $(WALLCLOCK_BIN)


install-arch:
	BIN=$(HOME2L_INSTALL)/bin/$(ARCH);   \
	install -Dps -m 755 -t $$BIN $(WALLCLOCK_BIN);


endif


build-indep: icons.build
ifeq ($(WITH_ANDROID),1)
build-indep: $(APK)
endif


install-indep:
	install -Dp -m 755 -t $(HOME2L_INSTALL)/bin home2l-fpc; \
	install -Dp -m 755 -t $(HOME2L_INSTALL)/bin home2l-icon; \
	install -Dp -m 755 -t $(HOME2L_INSTALL)/bin h2l-sysinfo.sh; \
	if [[ "$(WITH_ANDROID)" == "1" ]]; then \
	  install -Dp -m 644 $(APK) $(HOME2L_INSTALL)/install/Home2lApp.apk; \
	fi; \
	install -Dp -m 644 -t $(HOME2L_INSTALL)/share/icons icons.build/*.bmp; \
	install -Dp -m 644 -t $(HOME2L_INSTALL)/share/fonts fonts/*.ttf; \
	install -Dp -m 644 -t $(HOME2L_INSTALL)/share/sounds sounds/*.wav; \
	install -Dp -m 644 -T icons/home2l-desktop.png $(HOME2L_INSTALL)/share/home2l.png; \
	echo -e "[Desktop Entry]\n" \
	        "Name=Home2L\n" \
	        "Comment=The Home2L WallClock\n" \
	        "Comment[de]=Die Home2L-Wanduhr\n" \
	        "Exec=home2l wallclock\n" \
	        "Icon=$(HOME2L_INSTALL)/share/home2l.png\n" \
	        "Terminal=false\n" \
	        "Type=Application\n" \
	        "Categories=Tools;\n" \
	  | sed 's#^ *##' > $(HOME2L_INSTALL)/share/home2l.desktop
