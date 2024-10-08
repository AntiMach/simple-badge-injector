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

RUN_FLAGS :=

VERSION_MAJOR := 1
VERSION_MINOR := 4
VERSION_MICRO := 2

APP_ROOT := /3ds/$(subst $() $(),,$(NAME))
APP_TITLE := $(NAME) $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO)

BUILD_FLAGS := -D__3DS__ "-DAPP_TITLE=\"$(APP_TITLE)\"" "-DAPP_ROOT=\"$(APP_ROOT)\""

# 3DS/Wii U CONFIGURATION #
ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    TITLE := $(APP_TITLE)
    DESCRIPTION := Inject, dump or delete badge data
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
