LOCAL_DIR := $(GET_LOCAL_DIR)

DEBUG := 2

ARCH := arm
ARM_CPU := cortex-a8
CPU := generic

DEFINES  += ARM_CPU_CORE_SCORPION PLATFORM_MSM7X30=1

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/target/$(TARGET)/include


DEVS += fbcon
MODULES += dev/fbcon

OBJS += \
	$(LOCAL_DIR)/interrupts.o \
	$(LOCAL_DIR)/acpuclock.o \
	$(LOCAL_DIR)/panel.o \
	$(LOCAL_DIR)/display.o \
	$(LOCAL_DIR)/gpio.o \
	$(LOCAL_DIR)/platform.o

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

include platform/msm_shared/rules.mk

