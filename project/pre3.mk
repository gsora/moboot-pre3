# top level project rules for the pre3 project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := pre3

MODULES += app/moboot
MODULES += lib/gfx
MODULES += lib/font
MODULES += lib/gfxconsole
MODULES += lib/atags
MODULES += lib/fs
MODULES += lib/bio
MODULES += lib/tga
MODULES += lib/ramdisk
MODULES += lib/zlib
MODULES += lib/bootlinux
MODULES += lib/uimage

#DEFINES += WITH_DEBUG_DCC=1
#DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1

DEFINES += _EMMC_BOOT=1
DEFINES += DISABLE_MMC_DEBUG_SPEW
DEFINES += HANDLE_LINUX_KERNEL_ARGS
DEFINES += WITH_LIB_FS_EXT2=1
DEFINES += MOBOOT_VERSION="\"0.3.5-pre3\""
