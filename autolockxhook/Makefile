include $(THEOS)/makefiles/common.mk

TWEAK_NAME = AutoLockX

$(TWEAK_NAME)_FILES = /mnt/d/codes/autolockx/autolockxhook/AutoLockX.xm
$(TWEAK_NAME)_FRAMEWORKS = CydiaSubstrate UIKit IOKit GraphicsServices
$(TWEAK_NAME)_PRIVATE_FRAMEWORKS = MediaRemote
$(TWEAK_NAME)_LDFLAGS = -Wl,-segalign,4000
$(TWEAK_NAME)_CFLAGS = -fobjc-arc

export ARCHS = armv7 armv7s arm64 arm64e
$(TWEAK_NAME)_ARCHS = armv7 armv7s arm64 arm64e

include $(THEOS_MAKE_PATH)/tweak.mk