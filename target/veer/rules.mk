LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared  -I$(LK_TOP_DIR)/platform/veer

PLATFORM := veer

MEMBASE := 0x09208000 # EBI
MEMSIZE := 0x00100000 # 1MB

BASE_ADDR            := 0x09200000

TAGS_ADDR            := BASE_ADDR+0x00000100
KERNEL_ADDR          := BASE_ADDR+0x00008000
RAMDISK_ADDR         := BASE_ADDR+0x01000000
SCRATCH_ADDR         := 0x01000000
SCRATCH_SIZE	     := 64
FASTBOOT_BUF_SIZE    := 0x07800000



DEFINES += \
	SDRAM_SIZE=$(MEMSIZE) \
	MEMBASE=$(MEMBASE) \
	BASE_ADDR=$(BASE_ADDR) \
	TAGS_ADDR=$(TAGS_ADDR) \
	KERNEL_ADDR=$(KERNEL_ADDR) \
	RAMDISK_ADDR=$(RAMDISK_ADDR) \
	SCRATCH_ADDR=$(SCRATCH_ADDR) \
	SCRATCH_SIZE=$(SCRATCH_SIZE) \
	FASTBOOT_BUF_SIZE=$(FASTBOOT_BUF_SIZE) \
	MACH_TYPE=2507

DEFINES += DISPLAY_TYPE_MDDI=1 TARGET_MSM7630_SURF MDP4=1

OBJS += \
	$(LOCAL_DIR)/gpiokeys.o \
	$(LOCAL_DIR)/atags.o \
	$(LOCAL_DIR)/init.o
