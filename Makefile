# TARGET #

TARGET := 3DS
LIBRARY := 0

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    ifeq ($(strip $(DEVKITPRO)),)
        $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
    endif
endif

# COMMON CONFIGURATION #

NAME := Simple Badge Injector

BUILD_DIR := build
OUTPUT_DIR := output
INCLUDE_DIRS := include
SOURCE_DIRS := source


EXTRA_OUTPUT_FILES :=

LIBRARY_DIRS := 
LIBRARIES := 

BUILD_FLAGS := 
RUN_FLAGS :=

VERSION_MAJOR := 1
VERSION_MINOR := 3
VERSION_MICRO := 0

# 3DS/Wii U CONFIGURATION #

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    TITLE := $(NAME)
    DESCRIPTION := An app that injects badge data
    AUTHOR := AntiMach
endif

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    LIBRARY_DIRS += $(DEVKITPRO)/libctru
    LIBRARIES += ctru

    PRODUCT_CODE := CTR-P-SBIN
    UNIQUE_ID := 0x5B1

    CATEGORY := Application
    USE_ON_SD := true

    MEMORY_TYPE := Application
    SYSTEM_MODE := 64MB
    SYSTEM_MODE_EXT := Legacy
    CPU_SPEED := 268MHz
    ENABLE_L2_CACHE := true
	
	ICON_FLAGS :=
	
    # ROMFS_DIR :=
    BANNER_AUDIO := meta/banner.wav
    BANNER_IMAGE := meta/banner.png
    ICON := meta/icon.png
    LOGO := meta/logo.bcma.lz
endif

# INTERNAL #

include buildtools/make_base
